#pragma once

#include "vulkan/vk_context.hpp"

void pickPhysicalDevice(VkContext& context);
vk::Device createDevice(VkContext& vkContext);
