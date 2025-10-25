/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "vulkan/vk_image.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_texture.hpp"
#include "vulkan/vk_command.hpp"
#include "vulkan/vk_buffer.hpp"
#include <vulkan/vulkan_core.h>

void createImageViews(VkContext& context)
{
	context.swapChainImageViews.clear();

	vk::ImageViewCreateInfo imageViewCreateInfo {
		.viewType = vk::ImageViewType::e2D,
		.format = context.swapChainImageFormat,
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	for (auto image : context.swapChainImages)
	{
		imageViewCreateInfo.image = image;
		context.swapChainImageViews.push_back(context.device.createImageView(imageViewCreateInfo));
	}
}

vk::ImageView createImageView(VkContext& context, vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo {
		.image = image,
		.viewType = vk::ImageViewType::e2D,
		.format = format,
		.subresourceRange = {
			.aspectMask = aspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	return context.device.createImageView(viewInfo);
}

void createTextureImageView(VkContext& context)
{
	context.textureImageView = createImageView(context, context.textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void createDepthResources(VkContext& context)
{
	vk::Format depthFormat = findDepthFormat(context);
	createImage(context, context.swapChainExtent.width, context.swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, context.depthImage, context.depthImageMemory);

	context.depthImageView = createImageView(context, context.depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

vk::Format findSupportedFormat(VkContext& context, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for (const auto format : candidates) {
		vk::FormatProperties props = context.gpu.getFormatProperties(format);

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		}

		if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	throw std::runtime_error("Failed to find supported format!");
}

vk::Format findDepthFormat(VkContext& context)
{
	return findSupportedFormat(
		context,
		{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment
	);
}

bool hasStencilComponent(vk::Format format)
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void createImage(VkContext& context,
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Image& image,
		vk::DeviceMemory& imageMemory)
{
	vk::ImageCreateInfo imageInfo {
		.flags = {},
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = { width, height, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.initialLayout = vk::ImageLayout::eUndefined
	};

	image = context.device.createImage(imageInfo);
	vk::MemoryRequirements memRequirements = context.device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo {
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(context, memRequirements.memoryTypeBits, properties)
	};

	imageMemory = context.device.allocateMemory(allocInfo);
	context.device.bindImageMemory(image, imageMemory, 0);
}

void transitionImageLayout(VkContext& context, const vk::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	auto commandBuffer = beginSingleTimeCommands(context);
	vk::ImageMemoryBarrier barrier {
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.image = image,
		.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
	};

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else {
		throw std::invalid_argument("Unsupported layout transition!");
	}

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
	endSingleTimeCommands(context, commandBuffer);
}
