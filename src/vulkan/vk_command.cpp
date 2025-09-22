/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "vulkan/vk_command.hpp"
#include <vulkan/vulkan_structs.hpp>

void createCommandPool(VkContext& context)
{
	vk::CommandPoolCreateInfo poolInfo {
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = context.graphicsFamily.value()
	};
	context.commandPool = context.device.createCommandPool(poolInfo);
}

void createCommandBuffer(VkContext& context)
{
	vk::CommandBufferAllocateInfo allocInfo {
		.commandPool = context.commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	context.commandBuffer = std::move(context.device.allocateCommandBuffers(allocInfo)).front();
}

void transition_image_layout(
	VkContext& context,
        uint32_t currentFrame,
        vk::ImageLayout old_layout,
        vk::ImageLayout new_layout,
        vk::AccessFlags2 src_access_mask,
        vk::AccessFlags2 dst_access_mask,
        vk::PipelineStageFlags2 src_stage_mask,
        vk::PipelineStageFlags2 dst_stage_mask)
{
        vk::ImageMemoryBarrier2 barrier = {
		.srcStageMask = src_stage_mask,
		.srcAccessMask = src_access_mask,
		.dstStageMask = dst_stage_mask,
		.dstAccessMask = dst_access_mask,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = context.swapChainImages[currentFrame],
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	vk::DependencyInfo dependency_info = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier
	};

	context.commandBuffer.pipelineBarrier2(dependency_info);
}

void recordCommandBuffer(VkContext& context, uint32_t imageIndex)
{
	vk::Result result = context.commandBuffer.begin({});
	if (result != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to begin command buffer recording!");
	}

	transition_image_layout(
		context,
		imageIndex,
				vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eTopOfPipe,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::RenderingAttachmentInfo attachmentInfo = {
		.imageView = context.swapChainImageViews[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.clearValue = clearColor
	};

	vk::RenderingInfo renderingInfo = {
		.renderArea = { .offset = { 0, 0 }, .extent = context.swapChainExtent },
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentInfo
	};

	context.commandBuffer.beginRendering(renderingInfo);
        // context.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *context.graphicsPipeline);
        context.commandBuffer.setViewport(
		0,
		vk::Viewport(0.0f, 0.0f, static_cast<float>(context.swapChainExtent.width),
			static_cast<float>(context.swapChainExtent.height),
			0.0f,
			1.0f
		)
	);
        context.commandBuffer.setScissor( 0, vk::Rect2D( vk::Offset2D(0, 0), context.swapChainExtent));
        context.commandBuffer.draw(3, 1, 0, 0);
        context.commandBuffer.endRendering();

	transition_image_layout(
		context,
		imageIndex,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		{},
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eBottomOfPipe
	);

        context.commandBuffer.end();
}
