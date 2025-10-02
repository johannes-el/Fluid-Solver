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

	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	std::vector<vk::ImageView> swapChainImageViews;
	vk::Format                 swapChainImageFormat = vk::Format::eUndefined;
	std::vector<vk::Image>     swapChainImages;
	std::vector<vk::Semaphore> recycledSemaphores;

	vk::Pipeline               graphicsPipeline = nullptr;
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

	vk::Buffer vertexBuffer                = nullptr;
	vk::DeviceMemory vertexBufferMemory    = nullptr;
};

void initWindow(VkContext& context, AppConfig& config);
void initVulkan(VkContext& context);
void drawFrame(VkContext& context);
void run(VkContext& context);
void cleanup(VkContext& context);
