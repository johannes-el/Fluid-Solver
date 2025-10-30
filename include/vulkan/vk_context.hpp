/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#pragma once

#include "includes.hpp"
#include "app_config.hpp"
#include <vulkan/vulkan_handles.hpp>
#include "vk_mem_alloc.h"
#include "vulkan/vk_vertex.hpp"

class ImGuiVulkanUtil;

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct VkContext {
	vk::Instance instance      = nullptr;
	vk::PhysicalDevice gpu     = nullptr;
	vk::Device device          = nullptr;
	vk::Queue graphicsQueue    = nullptr;
	vk::Queue presentQueue     = nullptr;
	vk::SwapchainKHR swapChain = nullptr;
	vk::SurfaceKHR surface     = nullptr;

	vk::SurfaceFormatKHR swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;

	VmaAllocator allocator;

	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	std::vector<vk::ImageView> swapChainImageViews;
	vk::Format                 swapChainImageFormat = vk::Format::eUndefined;
	std::vector<vk::Image>     swapChainImages;
	std::vector<vk::Semaphore> recycledSemaphores;

	vk::Pipeline               graphicsPipeline = nullptr;
	vk::DescriptorSetLayout descriptorSetLayout = nullptr;
	vk::PipelineLayout         pipelineLayout   = nullptr;
	vk::DebugUtilsMessengerEXT debugCallback    = nullptr;

	vk::CommandPool commandPool = nullptr;
	std::vector<vk::CommandBuffer> commandBuffers;

	vk::ShaderModule shaderModule;

	std::vector<vk::Semaphore> presentCompleteSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	std::vector<vk::Fence> inFlightFences;

	GLFWwindow* window                     = nullptr;
	uint32_t currentFrame = 0;

	bool framebufferResized = false;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	vk::Buffer vertexBuffer                = nullptr;
	vk::DeviceMemory vertexBufferMemory    = nullptr;

	vk::Buffer indexBuffer                 = nullptr;
	vk::DeviceMemory indexBufferMemory     = nullptr;

	std::vector<vk::Buffer> uniformBuffers;
	std::vector<vk::DeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	vk::DescriptorPool descriptorPool      = nullptr;
	vk::DescriptorPool imguiPool;

	std::vector<vk::DescriptorSet> descriptorSets;

	vk::Image textureImage = nullptr;
	vk::ImageView textureImageView = nullptr;
	vk::Sampler textureSampler = nullptr;
	vk::DeviceMemory textureImageMemory = nullptr;

	vk::Image depthImage = nullptr;
	vk::DeviceMemory depthImageMemory = nullptr;
	vk::ImageView depthImageView = nullptr;
	std::unique_ptr<ImGuiVulkanUtil> imGui;
};

void initWindow(VkContext& context, AppConfig& config);
void initVulkan(VkContext& context);
void drawFrame(VkContext& context);
void run(VkContext& context);
void cleanup(VkContext& context);
