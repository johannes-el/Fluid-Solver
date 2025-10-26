#include "includes.hpp"
#include "vulkan/vk_descriptor.hpp"
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_uniforms.hpp"
#include <vulkan/vulkan_structs.hpp>

void createDescriptorSetLayout(VkContext& context)
{
    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {
	    vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),
	    vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr)
    };

    vk::DescriptorSetLayoutCreateInfo layoutInfo {
	    .bindingCount = static_cast<uint32_t>(bindings.size()),
	    .pBindings = bindings.data()
    };

    context.descriptorSetLayout = context.device.createDescriptorSetLayout(layoutInfo);
}

void createDescriptorPool(VkContext& context)
{
	std::array<vk::DescriptorPoolSize, 2> poolSize {
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT),
		vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, MAX_FRAMES_IN_FLIGHT)
	};
	vk::DescriptorPoolCreateInfo poolInfo {
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = MAX_FRAMES_IN_FLIGHT,
		.poolSizeCount = static_cast<uint32_t>(poolSize.size()),
		.pPoolSizes = poolSize.data()
	};

	context.descriptorPool = context.device.createDescriptorPool(poolInfo);
}

void createDescriptorSets(VkContext& context)
{
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, context.descriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo{ .descriptorPool = context.descriptorPool, .descriptorSetCount = static_cast<uint32_t>(layouts.size()), .pSetLayouts = layouts.data() };

	context.descriptorSets.clear();
	context.descriptorSets = context.device.allocateDescriptorSets(allocInfo);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vk::DescriptorBufferInfo bufferInfo{ .buffer = context.uniformBuffers[i], .offset = 0, .range = sizeof(UniformBufferObject) };
		vk::DescriptorImageInfo imageInfo(context.textureSampler, context.textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal);

		std::array<vk::WriteDescriptorSet, 2> descriptorWrites {
			vk::WriteDescriptorSet {
				.dstSet = context.descriptorSets[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eUniformBuffer,
				.pBufferInfo = &bufferInfo
			},
			vk::WriteDescriptorSet {
				.dstSet = context.descriptorSets[i],
				.dstBinding = 1,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eCombinedImageSampler,
				.pImageInfo = &imageInfo
			}
		};
		context.device.updateDescriptorSets(descriptorWrites, {});
	}
}
