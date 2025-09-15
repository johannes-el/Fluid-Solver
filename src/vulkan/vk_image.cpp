/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "vulkan/vk_image.hpp"
#include <vulkan/vulkan_core.h>

void createImageViews(VkContext& context)
{
	context.swapChainImageViews.clear();

	vk::ImageViewCreateInfo imageViewCreateInfo {
		.viewType = vk::ImageViewType::e2D,
		.format = context.swapChainImageFormat,
		.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
	};

	for (auto image : context.swapChainImages)
	{
		imageViewCreateInfo.image = image;
		context.swapChainImageViews.push_back(context.device.createImageView(imageViewCreateInfo));
	}
}
