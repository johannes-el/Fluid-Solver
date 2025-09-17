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
#include "vulkan/vk_instance.hpp"
#include "vulkan/vk_device.hpp"
#include <vulkan/vulkan_handles.hpp>

void initWindow(VkContext& context, AppConfig& config)
{
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
	createDevice(context);

	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(
			context.instance,
			context.window,
			nullptr,
			&_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}
	context.surface = vk::SurfaceKHR(_surface);
}

void run(VkContext& context)
{
	while (!glfwWindowShouldClose(context.window)) {
		glfwPollEvents();
	}
}

void cleanup(VkContext& context)
{
	glfwDestroyWindow(context.window);
	glfwTerminate();
}
