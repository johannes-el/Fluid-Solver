 /*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

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
	createGraphicsPipeline(context);
	createCommandPool(context);
	createVertexBuffer(context);
	createCommandBuffers(context);
	createSyncObjects(context);
}

void drawFrame(VkContext& context) {
        context.device.waitIdle();

	auto [result, imageIndex] = context.device.acquireNextImageKHR (
		context.swapChain,
		UINT64_MAX,
		context.presentCompleteSemaphores[context.currentFrame],
		nullptr
	);

        recordCommandBuffer(context, imageIndex);

        context.device.resetFences(context.inFlightFences[context.currentFrame]);
        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        const vk::SubmitInfo submitInfo {
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &context.presentCompleteSemaphores[context.currentFrame],
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &context.commandBuffers[context.currentFrame],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &context.renderFinishedSemaphores[context.currentFrame]
	};
        context.graphicsQueue.submit(submitInfo, context.inFlightFences[context.currentFrame]);
        while ( vk::Result::eTimeout == context.device.waitForFences(context.inFlightFences[context.currentFrame], vk::True, UINT64_MAX))
            ;

        const vk::PresentInfoKHR presentInfoKHR {
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &context.renderFinishedSemaphores[context.currentFrame],
		.swapchainCount = 1,
		.pSwapchains = &context.swapChain,
		.pImageIndices = &imageIndex
	};

        result = context.presentQueue.presentKHR(presentInfoKHR);

        switch (result)
        {
            case vk::Result::eSuccess: break;
            case vk::Result::eSuboptimalKHR: std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n"; break;
            default: break;
        }

	context.currentFrame = (context.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void run(VkContext& context)
{
	while (!glfwWindowShouldClose(context.window)) {
		glfwPollEvents();
		drawFrame(context);
	}
	context.device.waitIdle();
}

void cleanup(VkContext& context)
{
	context.device.waitIdle();

	context.device.freeMemory(context.vertexBufferMemory);
	context.device.destroyBuffer(context.vertexBuffer);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (context.renderFinishedSemaphores[i]) {
			context.device.destroy(context.renderFinishedSemaphores[i]);
		}
		if (context.presentCompleteSemaphores[i]) {
			context.device.destroy(context.presentCompleteSemaphores[i]);
		}
		if (context.inFlightFences[i]) {
			context.device.destroy(context.inFlightFences[i]);
		}
	}

	context.device.destroy(context.commandPool);
	context.device.destroy(context.graphicsPipeline);
	context.device.destroy(context.pipelineLayout);

	context.device.destroy(context.shaderModule);

	for (auto &imageView : context.swapChainImageViews) {
		context.device.destroyImageView(imageView);
	}

	context.device.destroySwapchainKHR(context.swapChain);
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
