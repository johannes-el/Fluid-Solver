/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "imgui.h"
#include "includes.hpp"

#include "app_config.hpp"

#include "vulkan/vk_context.hpp"
#include "vulkan/vk_buffer.hpp"
#include "vulkan/vk_instance.hpp"
#include "vulkan/vk_device.hpp"
#include "vulkan/vk_swapchain.hpp"
#include "vulkan/vk_pipeline.hpp"
#include "vulkan/vk_image.hpp"
#include "vulkan/vk_command.hpp"
#include "vulkan/vk_sync.hpp"
#include "vulkan/vk_descriptor.hpp"
#include "vulkan/vk_texture.hpp"
#include "vulkan/vk_model.hpp"

#include "gui/imgui.hpp"
#include <cstdint>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <vulkan/vulkan_handles.hpp>

void initWindow(VkContext& context, AppConfig& config){
	if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW!");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	context.window = glfwCreateWindow(
		config.width,
		config.height,
		config.title.c_str(),
		nullptr,
		nullptr
		);
	glfwShowWindow(context.window);

	if (!context.window) throw std::runtime_error("Failed to create window!");
}

void initVulkan(VkContext& context)
{
	createInstance(context);
	setupValidationLayers(context);
	pickPhysicalDevice(context);
	createSurface(context);
	createDevice(context);
	createSwapChain(context);
	createImageViews(context);
	createDescriptorSetLayout(context);
	createGraphicsPipeline(context);
	createCommandPool(context);
	createDepthResources(context);
	createTextureImage(context);
	createTextureImageView(context);
	createTextureSampler(context);
	loadModel(context, "../models/avocado/Avocado.gltf");
	createVertexBuffer(context);
	createIndexBuffer(context);
	createUniformBuffers(context);
	createDescriptorPool(context);
	createDescriptorSets(context);
	createCommandBuffers(context);
	createSyncObjects(context);
}

void drawFrame(VkContext& context)
{
	vk::Result waitResult = context.device.waitForFences(
		{ context.inFlightFences[context.currentFrame] },
		true,
		UINT64_MAX
	);

	if (waitResult != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to wait for fence!");
	}

	auto [result, imageIndex] = context.device.acquireNextImageKHR(
		context.swapChain,
		UINT64_MAX,
		context.presentCompleteSemaphores[context.currentFrame],
		nullptr
	);

        if (result == vk::Result::eErrorOutOfDateKHR) {
		recreateSwapChain(context);
		return;
        }
        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
        }

	if (context.imGui->newFrame()) {
		context.imGui->updateBuffers();
	}

	context.imGui->newFrame();
	context.imGui->updateBuffers();

	updateUniformBuffer(context, context.currentFrame);

        context.device.resetFences(context.inFlightFences[context.currentFrame]);
        context.commandBuffers[context.currentFrame].reset();
        recordCommandBuffer(context, imageIndex);

	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	vk::SubmitInfo submitInfo{};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &context.presentCompleteSemaphores[context.currentFrame];
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &context.commandBuffers[context.currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &context.renderFinishedSemaphores[imageIndex];

	context.graphicsQueue.submit(submitInfo, context.inFlightFences[context.currentFrame]);

	vk::PresentInfoKHR presentInfo{};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &context.renderFinishedSemaphores[imageIndex];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &context.swapChain;
	presentInfo.pImageIndices = &imageIndex;

	result = context.presentQueue.presentKHR(presentInfo);

	if (result == vk::Result::eErrorOutOfDateKHR ||
		result == vk::Result::eSuboptimalKHR ||
		context.framebufferResized) {
		context.framebufferResized = false;
		recreateSwapChain(context);
	} else if (result != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to present swap chain image!");
	}

	context.currentFrame = (context.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void run(VkContext& context)
{
	using clock = std::chrono::high_resolution_clock;
	auto lastTime = clock::now();
	const auto frameDuration = std::chrono::milliseconds(16); // ~60 FPS

	while (!glfwWindowShouldClose(context.window)) {
		glfwPollEvents();
		drawFrame(context);

		auto now = clock::now();
		auto elapsed = now - lastTime;
		if (elapsed < frameDuration) {
			std::this_thread::sleep_for(frameDuration - elapsed);
		}
		lastTime = clock::now();
	}

	context.device.waitIdle();
}


void cleanup(VkContext& context)
{
	context.device.waitIdle();

	if (context.imGui) {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	if (context.depthImageView) context.device.destroyImageView(context.depthImageView);
	if (context.depthImage) context.device.destroyImage(context.depthImage);
	if (context.depthImageMemory) context.device.freeMemory(context.depthImageMemory);

	if (context.textureSampler) context.device.destroySampler(context.textureSampler);

	if (context.imguiPool) context.device.destroyDescriptorPool(context.imguiPool);

	context.device.destroyImage(context.textureImage);
	context.device.freeMemory(context.textureImageMemory);

	context.device.destroyDescriptorSetLayout(context.descriptorSetLayout);
	context.device.destroyDescriptorPool(context.descriptorPool);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (context.uniformBuffersMapped[i]) {
			context.device.unmapMemory(context.uniformBuffersMemory[i]);
		}

		context.device.destroyBuffer(context.uniformBuffers[i]);
		context.device.freeMemory(context.uniformBuffersMemory[i]);
	}

	context.uniformBuffers.clear();
	context.uniformBuffersMemory.clear();
	context.uniformBuffersMapped.clear();

	context.device.destroyBuffer(context.vertexBuffer);
	context.device.freeMemory(context.vertexBufferMemory);

	context.device.destroyBuffer(context.indexBuffer);
	context.device.freeMemory(context.indexBufferMemory);

	for (auto& semaphore : context.renderFinishedSemaphores) {
		context.device.destroySemaphore(semaphore);
	}
	for (auto& semaphore : context.presentCompleteSemaphores) {
		context.device.destroySemaphore(semaphore);
	}
	for (auto& fence : context.inFlightFences) {
		context.device.destroyFence(fence);
	}

	context.device.destroyCommandPool(context.commandPool);
	context.device.destroyPipeline(context.graphicsPipeline);
	context.device.destroyPipelineLayout(context.pipelineLayout);

	context.device.destroyShaderModule(context.shaderModule);

	for (auto& imageView : context.swapChainImageViews) {
		context.device.destroyImageView(imageView);
	}

	context.swapChainImageViews.clear();
	context.device.destroySwapchainKHR(context.swapChain);
	context.swapChainImages.clear();
	context.device.destroy();

	context.instance.destroySurfaceKHR(context.surface);

	if (enableValidationLayers && context.debugCallback != VK_NULL_HANDLE) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
			context.instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func) {
			func(context.instance, context.debugCallback, nullptr);
		}
	}

	context.instance.destroy();

	glfwDestroyWindow(context.window);
	glfwTerminate();
}
