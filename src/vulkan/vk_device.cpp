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

void createDevice(VkContext& context)
{
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = context.gpu.getQueueFamilyProperties();

	auto graphicsQueueFamilyProperty = std::ranges::find_if(
		queueFamilyProperties,
		[]( auto const & qfp )
		{ return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0); }
	);

	auto graphicsIndex = static_cast<uint32_t>(
		std::distance( queueFamilyProperties.begin(), graphicsQueueFamilyProperty)
	);

	// determine a queueFamilyIndex that supports present
	// first check if the graphicsIndex is good enough
	auto presentIndex = context.gpu.getSurfaceSupportKHR( graphicsIndex, context.surface )
		? graphicsIndex
		: static_cast<uint32_t>(queueFamilyProperties.size());

	if (presentIndex == queueFamilyProperties.size()) {
		for ( size_t i = 0; i < queueFamilyProperties.size(); i++ )
		{
			if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
				context.gpu.getSurfaceSupportKHR( static_cast<uint32_t>(i), context.surface )) {
				graphicsIndex = static_cast<uint32_t>( i );
				presentIndex  = graphicsIndex;
				break;
			}
		}

		if (presentIndex == queueFamilyProperties.size()) {
			for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
				if (context.gpu.getSurfaceSupportKHR( static_cast<uint32_t>(i),
									context.surface)) {
					presentIndex = static_cast<uint32_t>(i);
					break;
				}
			}
		}
	}
	if ((graphicsIndex == queueFamilyProperties.size()) || (presentIndex == queueFamilyProperties.size())) {
		throw std::runtime_error( "Could not find a queue for graphics or present -> terminating" );
	}

	auto features = context.gpu.getFeatures2();

	vk::PhysicalDeviceVulkan11Features vulkan11Features{};
	vk::PhysicalDeviceVulkan13Features vulkan13Features{};
	vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures{};
	vk::PhysicalDeviceFeatures2 enabledFeatures{};

	context.gpu.getFeatures2(&enabledFeatures);

	if (enabledFeatures.features.samplerAnisotropy) {
		enabledFeatures.features.samplerAnisotropy = VK_TRUE;
	}
	else {
		throw std::runtime_error("Anisotropic filtering not supported!");
	}

	vulkan11Features.shaderDrawParameters = vk::True;
	vulkan13Features.dynamicRendering = vk::True;
	vulkan13Features.synchronization2 = vk::True;
	extendedDynamicStateFeatures.extendedDynamicState = vk::True;

	vulkan13Features.pNext = &extendedDynamicStateFeatures;
	vulkan11Features.pNext = &vulkan13Features;
	enabledFeatures.pNext = &vulkan11Features;

	float                     queuePriority = 0.0f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo {
		.queueFamilyIndex = graphicsIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::DeviceCreateInfo      deviceCreateInfo {
		.pNext =  &enabledFeatures,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data()
	};

	context.graphicsFamily = graphicsIndex;
	context.presentFamily  = presentIndex;

	context.device = context.gpu.createDevice(deviceCreateInfo);
	context.graphicsQueue = context.device.getQueue(graphicsIndex, 0);
	context.presentQueue = context.device.getQueue(presentIndex, 0);
}
