/*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#pragma once

#include "vk_context.hpp"

void createSwapchain(VkContext& context);
void recreateSwapchain(VkContext& context);
void cleanupSwapchain(VkContext& context);

void get_queue_family_index(std::vector<vk::QueueFamilyProperties> const &queue_family_properties, vk::QueueFlagBits queue_flag);
vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
vk::Extent2D chooseSwapExtent(VkContext& context, const vk::SurfaceCapabilitiesKHR& capabilities);
