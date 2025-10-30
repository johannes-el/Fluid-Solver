#include "includes.hpp"
#include "vulkan/vk_buffer.hpp"
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_vertex.hpp"
#include "vulkan/vk_uniforms.hpp"
#include "vulkan/vk_command.hpp"
#include "vulkan/vulkan.hpp"
#include <chrono>
#include <glm/ext/matrix_clip_space.hpp>

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
	vk::BufferCreateInfo bufferInfo {
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};
	buffer = context.device.createBuffer(bufferInfo);

	vk::MemoryRequirements memRequirements = context.device.getBufferMemoryRequirements(buffer);

	uint32_t memoryTypeIndex = findMemoryType(context, memRequirements.memoryTypeBits, properties);

	vk::MemoryAllocateInfo allocInfo {
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
	vk::CommandBuffer commandCopyBuffer = beginSingleTimeCommands(context);
	commandCopyBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	endSingleTimeCommands(context, commandCopyBuffer);
}

void copyBufferToImage(VkContext& context, const vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands(context);
	vk::BufferImageCopy region = {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
		.imageOffset = {0, 0, 0},
		.imageExtent = {width, height, 1}
	};
	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {region});
	endSingleTimeCommands(context, commandBuffer);
}

void createVertexBuffer(VkContext& context)
{
	vk::DeviceSize bufferSize = sizeof(context.vertices[0]) * context.vertices.size();

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
	vk::MemoryAllocateInfo memoryAllocateInfoStaging {
		.allocationSize = memRequirementsStaging.size,
		.memoryTypeIndex = stagingMemTypeIndex
	};

	vk::DeviceMemory stagingBufferMemory = context.device.allocateMemory(memoryAllocateInfoStaging);
	context.device.bindBufferMemory(stagingBuffer, stagingBufferMemory, 0);

	void* dataStaging = context.device.mapMemory(stagingBufferMemory, 0, bufferSize);
	memcpy(dataStaging, context.vertices.data(), (size_t)bufferSize);
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
	vk::DeviceSize bufferSize = sizeof(context.indices[0]) * context.indices.size();

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
	memcpy(data, context.indices.data(), (size_t) bufferSize);
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

void createUniformBuffers(VkContext& context)
{
	context.uniformBuffers.clear();
	context.uniformBuffersMemory.clear();
	context.uniformBuffersMapped.clear();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
		vk::Buffer buffer{};
		vk::DeviceMemory bufferMem{};
		createBuffer(context,
			bufferSize,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			buffer,
			bufferMem
		);

		context.uniformBuffers.emplace_back(std::move(buffer));
		context.uniformBuffersMemory.emplace_back(std::move(bufferMem));
		context.uniformBuffersMapped.emplace_back(
			context.device.mapMemory(context.uniformBuffersMemory[i], 0, bufferSize)
		);
	}
}

void updateUniformBuffer(VkContext& context, uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};

	float aspect = static_cast<float>(context.swapChainExtent.width) / static_cast<float>(context.swapChainExtent.height);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(10.f));
	model = glm::rotate(model, time * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	ubo.model = model;

	ubo.view = glm::lookAt(glm::vec3(0.0f, 0.5f, 1.0f),
                        glm::vec3(0.0f, 0.2f, 0.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));


	ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
	ubo.proj[1][1] *= -1;

	memcpy(context.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
