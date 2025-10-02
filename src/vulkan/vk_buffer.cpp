#include "includes.hpp"
#include "vulkan/vk_buffer.hpp"
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_vertex.hpp"

uint32_t findMemoryType(VkContext& context, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = context.gpu.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}

void createVertexBuffer(VkContext& context)
{
	vk::BufferCreateInfo bufferInfo {
		.flags = {},
		.size = sizeof(vertices[0]) * vertices.size(),
		.usage = vk::BufferUsageFlags(vk::BufferUsageFlagBits::eVertexBuffer),
		.sharingMode = vk::SharingMode::eExclusive
	};

	context.vertexBuffer = context.device.createBuffer(bufferInfo);

	vk::MemoryRequirements memRequirements = context.device.getBufferMemoryRequirements(context.vertexBuffer);

	vk::MemoryAllocateInfo memoryAllocateInfo {
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType (
			context,
			memRequirements.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		)
	};

	context.vertexBufferMemory = context.device.allocateMemory(memoryAllocateInfo);
	context.device.bindBufferMemory(context.vertexBuffer, context.vertexBufferMemory, 0);

	void* data = context.device.mapMemory(context.vertexBufferMemory, 0, bufferInfo.size);
	memcpy(data, vertices.data(), bufferInfo.size);
	context.device.unmapMemory(context.vertexBufferMemory);
}
