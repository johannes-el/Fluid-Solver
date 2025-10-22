/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

//////////////////////////////////////////////////////////////////////////
/*
  Instance creation that describes the application and the API extensions.
  Vulkan Validation Layers are enabled and a
  suitable physical device is being selected.
*/
//////////////////////////////////////////////////////////////////////////

#include "vulkan/vk_instance.hpp"
#include "vulkan/vk_context.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <iostream>

void createSurface(VkContext& context)
{
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

vk::Bool32 debugCallback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT	messageSeverity,
	vk::DebugUtilsMessageTypeFlagsEXT               messageType,
	const vk::DebugUtilsMessengerCallbackDataEXT*   pCallbackData,
	void* /*pUserData*/)
{
	std::cerr << "Validation: " << pCallbackData->pMessage << "\n";
	return vk::False;
}

void setupValidationLayers(VkContext& vkContext)
{
	if (!enableValidationLayers) return;

	vk::DebugUtilsMessengerCreateInfoEXT createInfo{
		.messageSeverity =
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		.messageType =
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		.pfnUserCallback = debugCallback
	};

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
		vkContext.instance, "vkCreateDebugUtilsMessengerEXT");
	if (!func) throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT");

	VkDebugUtilsMessengerEXT messenger;
	VkResult result = func(static_cast<VkInstance>(vkContext.instance),
			reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo),
			nullptr, &messenger);

	if (result != VK_SUCCESS) throw std::runtime_error("Failed to create debug messenger");

	vkContext.debugCallback = messenger;
}

void createInstance(VkContext& vkContext)
{
	constexpr vk::ApplicationInfo appInfo {
		.pApplicationName   = "Fluid Solver",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName        = "No Engine",
		.engineVersion      = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion         = vk::ApiVersion14
	};

	// GLFW instance extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);
	}

	// Enable surface extensions depending on os
#if defined(_WIN32)
	requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	requiredExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_DIRECT2DISPLAY)
	requiredExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	requiredExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	requiredExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
	requiredExtensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	requiredExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif

	std::vector<const char*> requiredLayers;
	if (enableValidationLayers) {
		requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
	}

	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo {
		.messageSeverity =
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		.messageType =
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		.pfnUserCallback = debugCallback
	};

	vk::InstanceCreateInfo createInfo {
		.sType                   = vk::StructureType::eInstanceCreateInfo,
		.pNext                   = enableValidationLayers ? &debugCreateInfo : nullptr,
		.flags                   = {},
		.pApplicationInfo        = &appInfo,
		.enabledLayerCount       = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames     = requiredLayers.data(),
		.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()
	};

	vkContext.instance = vk::createInstance(createInfo);

	if (!vkContext.instance) {
		throw std::runtime_error("Failed to create Vulkan instance!");
	}
}
