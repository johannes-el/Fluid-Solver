#pragma once

#include "vk_context.hpp"

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

void createVertexBuffer(VkContext& context);
void createIndexBuffer(VkContext& context);
void createUniformBuffers(VkContext& context);

void updateUniformBuffer(VkContext& context, uint32_t currentImage);
