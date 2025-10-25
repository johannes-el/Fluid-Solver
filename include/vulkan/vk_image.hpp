/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#pragma once

#include "vk_context.hpp"

vk::ImageView createImageView(VkContext& context, vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags);
void createImageViews(VkContext& context);
void createTextureImageView(VkContext& context);

void createDepthResources(VkContext& context);
vk::Format findSupportedFormat(VkContext& context, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
vk::Format findDepthFormat(VkContext& context);

void createImage(VkContext& context,
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Image& image,
		vk::DeviceMemory& imageMemory
);

void transitionImageLayout(VkContext& context, const vk::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
