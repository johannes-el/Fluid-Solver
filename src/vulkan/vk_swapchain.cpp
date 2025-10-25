/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "includes.hpp"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "vulkan/vk_swapchain.hpp"
#include <vulkan/vk_image.hpp>

void createSwapChain(VkContext& context)
{
	auto surfaceCapabilities = context.gpu.getSurfaceCapabilitiesKHR(context.surface);
	context.swapChainExtent = chooseSwapExtent(context, surfaceCapabilities);
	context.swapChainSurfaceFormat = chooseSwapSurfaceFormat(
		context.gpu.getSurfaceFormatsKHR(context.surface)
	);

	auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
	minImageCount = (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount)
		? surfaceCapabilities.maxImageCount
		: minImageCount;

	uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
		imageCount = surfaceCapabilities.maxImageCount;
	}

	std::vector<vk::SurfaceFormatKHR> availableFormats = context.gpu.getSurfaceFormatsKHR(context.surface);
	std::vector<vk::PresentModeKHR> availablePresentModes = context.gpu.getSurfacePresentModesKHR(context.surface);

	vk::PresentModeKHR presentMode = chooseSwapPresentMode(availablePresentModes);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo {
		.surface = context.surface,
		.minImageCount = imageCount,
		.imageFormat = context.swapChainSurfaceFormat.format,
		.imageColorSpace = context.swapChainSurfaceFormat.colorSpace,
		.imageExtent = context.swapChainExtent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = presentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = nullptr
	};

	if (!context.graphicsFamily.has_value() || !context.presentFamily.has_value()) {
		throw std::runtime_error("Missing required queue family indices!");
	}

	// Decide on sharing mode
	uint32_t queueFamilyIndices[] = {context.graphicsFamily.value(), context.presentFamily.value()};
	if (context.graphicsFamily != context.presentFamily) {
		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}

	// Create the swapchain
	// context.swapChain = context.device.createSwapchainKHR(swapChainCreateInfo);
	try {
		context.swapChain = context.device.createSwapchainKHR(swapChainCreateInfo);
	} catch (vk::SystemError& err) {
		std::cerr << "Failed to create object: " << err.what() << std::endl;
	}

	context.swapChainImages = context.device.getSwapchainImagesKHR(context.swapChain);

	// Store chosen image format for later use
	context.swapChainImageFormat = context.swapChainSurfaceFormat.format;
}

void recreateSwapChain(VkContext& context)
{
	context.device.waitIdle();
	cleanupSwapchain(context);

	createSwapChain(context);
	createImageViews(context);
	createDepthResources(context);
}

void cleanupSwapchain(VkContext& context)
{
	for (auto& imageView : context.swapChainImageViews) {
		context.device.destroyImageView(imageView);
	}
	context.swapChainImageViews.clear();

	context.device.destroySwapchainKHR(context.swapChain);
	context.swapChainImages.clear();
	context.swapChain = nullptr;
}

void get_queue_family_index(std::vector<vk::QueueFamilyProperties> const &queue_family_properties, vk::QueueFlagBits queue_flag)
{

}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb
			&& availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
			return availablePresentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapExtent(VkContext& context, const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(context.window, &width, &height);

	return {
		std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
	};
}
