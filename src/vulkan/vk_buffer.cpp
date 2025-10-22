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

void createBuffer(VkContext& context,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer,
		vk::DeviceMemory& bufferMemory)
{
	vk::BufferCreateInfo bufferInfo{
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};
	buffer = context.device.createBuffer(bufferInfo);

	vk::MemoryRequirements memRequirements = context.device.getBufferMemoryRequirements(buffer);

	uint32_t memoryTypeIndex = findMemoryType(context, memRequirements.memoryTypeBits, properties);

	vk::MemoryAllocateInfo allocInfo{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = memoryTypeIndex
	};
	bufferMemory = context.device.allocateMemory(allocInfo);

	context.device.bindBufferMemory(buffer, bufferMemory, 0);
}

void copyBuffer(VkContext& context,
                vk::Buffer& srcBuffer,
                vk::Buffer& dstBuffer,
                vk::DeviceSize size)
{
	vk::CommandBufferAllocateInfo allocInfo {
		.commandPool = context.commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	std::vector<vk::CommandBuffer> commandBuffers = context.device.allocateCommandBuffers(allocInfo);
	vk::CommandBuffer commandBuffer = commandBuffers[0];

	vk::CommandBufferBeginInfo beginInfo {
		.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
	};
	commandBuffer.begin(beginInfo);

	vk::BufferCopy copyRegion{ 0, 0, size };
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	commandBuffer.end();

	vk::SubmitInfo submitInfo{
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer
	};

	vk::Result result = context.graphicsQueue.submit(1, &submitInfo, nullptr);
	if (result != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to submit command buffer to graphics queue!");
	}

	context.graphicsQueue.waitIdle();

	context.device.freeCommandBuffers(context.commandPool, 1, &commandBuffer);
}

void createVertexBuffer(VkContext& context)
{
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	vk::BufferCreateInfo stagingInfo {
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferSrc,
		.sharingMode = vk::SharingMode::eExclusive
	};

	vk::Buffer stagingBuffer = context.device.createBuffer(stagingInfo);

	vk::MemoryRequirements memRequirementsStaging = context.device.getBufferMemoryRequirements(stagingBuffer);
	uint32_t stagingMemTypeIndex = findMemoryType(
		context,
		memRequirementsStaging.memoryTypeBits,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);
	vk::MemoryAllocateInfo memoryAllocateInfoStaging{
		.allocationSize = memRequirementsStaging.size,
		.memoryTypeIndex = stagingMemTypeIndex
	};

	vk::DeviceMemory stagingBufferMemory = context.device.allocateMemory(memoryAllocateInfoStaging);
	context.device.bindBufferMemory(stagingBuffer, stagingBufferMemory, 0);

	void* dataStaging = context.device.mapMemory(stagingBufferMemory, 0, bufferSize);
	memcpy(dataStaging, vertices.data(), (size_t)bufferSize);
	context.device.unmapMemory(stagingBufferMemory);

	vk::BufferCreateInfo bufferInfo {
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		.sharingMode = vk::SharingMode::eExclusive
	};
	context.vertexBuffer = context.device.createBuffer(bufferInfo);

	vk::MemoryRequirements memRequirements = context.device.getBufferMemoryRequirements(context.vertexBuffer);
	uint32_t vertexMemTypeIndex = findMemoryType(
		context,
		memRequirements.memoryTypeBits,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	vk::MemoryAllocateInfo memoryAllocateInfo {
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = vertexMemTypeIndex
	};
	context.vertexBufferMemory = context.device.allocateMemory(memoryAllocateInfo);
	context.device.bindBufferMemory(context.vertexBuffer, context.vertexBufferMemory, 0);

	copyBuffer(context, stagingBuffer, context.vertexBuffer, bufferSize);

	context.device.destroyBuffer(stagingBuffer);
	context.device.freeMemory(stagingBufferMemory);
}


void createIndexBuffer(VkContext& context)
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	vk::Buffer stagingBuffer{};
	vk::DeviceMemory stagingBufferMemory{};

	createBuffer(
		context,
		bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBuffer,
		stagingBufferMemory
	);

	void* data = context.device.mapMemory(stagingBufferMemory, 0, bufferSize);
	memcpy(data, indices.data(), (size_t) bufferSize);
	context.device.unmapMemory(stagingBufferMemory);

	createBuffer(context,
		bufferSize,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		context.indexBuffer,
		context.indexBufferMemory);

	copyBuffer(context, stagingBuffer, context.indexBuffer, bufferSize);

	context.device.destroyBuffer(stagingBuffer);
	context.device.freeMemory(stagingBufferMemory);
}
