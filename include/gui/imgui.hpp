#pragma once

#include <imgui.h>

#include "includes.hpp"
#include "vulkan/vk_context.hpp"
#include "vulkan/vk_image.hpp"

class ImGuiVulkanUtil {
public:
	ImGuiVulkanUtil() = default;
	ImGuiVulkanUtil(vk::Device& device, vk::PhysicalDevice physialDevice,
			vk::Queue& graphicsQueue, uint32_t graphicsQueueFamily);

	~ImGuiVulkanUtil();

	void init(VkContext& context, float width, float height);
	void initResources();
	void setStyle(uint32_t index);

	bool newFrame();
	void updateBuffers();
	void drawFrame(vk::CommandBuffer& commandBuffer);

	void handleKey(int key, int scancode, int action, int mods);
	void charPressed(uint32_t key);

private:
	vk::Sampler sampler{nullptr};
	vk::Buffer vertexBuffer;
	vk::Buffer indexBuffer;

	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;
	vk::Image fontImage;
	vk::ImageView fontImageView;

	// Vulkan pipline infrastructure for UI rendering
	vk::PipelineCache piplineCache{nullptr};
	vk::PipelineLayout pipelineLayout{nullptr};
	vk::Pipeline pipeline{nullptr};
	vk::DescriptorPool descriptorPool{nullptr};
	vk::DescriptorSetLayout descriptorSetLayout{nullptr};
	vk::DescriptorSet descriptorSet{nullptr};

	// Vulkan device context and system integration
	vk::Device device;
	vk::PhysicalDevice physicalDevice;
	vk::Queue graphicsQueue;
	uint32_t graphicsQueueFamily = 0;

	// These members control the visual appearance and dynamic behavior of the UI system
	ImGuiStyle vulkanStyle;

	struct PushConstBlock {
		glm::vec2 scale;
		glm::vec2 translate;
	} pushConstBlock;

	bool needsUpdateBuffers = false;

	vk::PipelineRenderingCreateInfo renderingInfo{};
	vk::Format colorFormat = vk::Format::eB8G8R8A8Unorm;
};
