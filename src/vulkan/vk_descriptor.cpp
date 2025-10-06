#include "vulkan/vk_descriptor.hpp"
#include "vulkan/vk_context.hpp"
#include <vulkan/vulkan_structs.hpp>


void createDescriptorPool(VkContext& context)
{
	vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT);
	vk::DescriptorPoolCreateInfo poolInfo {
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = MAX_FRAMES_IN_FLIGHT,
		.poolSizeCount = 1,
		.pPoolSizes = &poolSize
	};

	// std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *context.descriptorSetLayout);

	context.descriptorSets.clear();
	// context.descriptorSets = context.device.allocateDescriptorSets(allocInfo);
}
