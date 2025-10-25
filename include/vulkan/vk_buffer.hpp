#pragma once

#include "vk_context.hpp"

uint32_t findMemoryType(VkContext& context, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
void createBuffer(VkContext& context,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer,
		vk::DeviceMemory& bufferMemory);
void copyBuffer(VkContext& context,
		vk::Buffer& srcBuffer,
		vk::Buffer& dstBuffer,
		vk::DeviceSize size);

void copyBufferToImage(VkContext& context, const vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height);

void createVertexBuffer(VkContext& context);
void createIndexBuffer(VkContext& context);
void createUniformBuffers(VkContext& context);

void updateUniformBuffer(VkContext& context, uint32_t currentImage);
