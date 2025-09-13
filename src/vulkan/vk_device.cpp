/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "vulkan/vk_context.hpp"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>

static const std::vector<const char*> deviceExtensions = {
	vk::KHRSwapchainExtensionName,
	vk::KHRSpirv14ExtensionName,
	vk::KHRSynchronization2ExtensionName,
	vk::KHRCreateRenderpass2ExtensionName
};

void pickPhysicalDevice(VkContext& vkContext)
{
	auto physicalDevices = vkContext.instance.enumeratePhysicalDevices();
	if (physicalDevices.empty())
		throw std::runtime_error("Failed to find a GPU with Vulkan support!");

	std::multimap<int, vk::PhysicalDevice> candidates;

	for (const auto& device : physicalDevices) {
		auto props = device.getProperties();
		auto feats = device.getFeatures();

		int score = 0;
		if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 1000;
		score += props.limits.maxImageDimension2D;

		if (!feats.geometryShader || props.apiVersion < VK_API_VERSION_1_4)
			continue;

		candidates.insert({score, device});
	}

	if (candidates.empty())
		throw std::runtime_error("Failed to find a suitable GPU!");

	vkContext.gpu = candidates.rbegin()->second;
}

uint32_t findGraphicsQueueFamily(vk::PhysicalDevice device)
{
	auto queueFamilies = device.getQueueFamilyProperties();
	auto it = std::find_if(queueFamilies.begin(), queueFamilies.end(),
			[](vk::QueueFamilyProperties const& qfp) {
				return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
			});
	if (it == queueFamilies.end())
		throw std::runtime_error("No graphics queue found!");
	return static_cast<uint32_t>(std::distance(queueFamilies.begin(), it));
}

vk::Device createDevice(VkContext& vkContext)
{
	uint32_t graphicsQueueFamily = findGraphicsQueueFamily(vkContext.gpu);

	float queuePriority = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo {
		.queueFamilyIndex = graphicsQueueFamily,
		.queueCount       = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::PhysicalDeviceFeatures deviceFeatures{};

	vk::DeviceCreateInfo createInfo {
		.queueCreateInfoCount    = 1,
		.pQueueCreateInfos       = &queueCreateInfo,
		.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures        = &deviceFeatures
	};

	vkContext.device = vkContext.gpu.createDevice(createInfo);
	vkContext.queue  = vkContext.device.getQueue(graphicsQueueFamily, 0);

	return vkContext.device;
}
