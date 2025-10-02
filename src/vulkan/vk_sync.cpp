 /*
 * Copyright (c) 2025 Johannes Elsing
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
 * You may not use this work for commercial purposes.
 * You must give appropriate credit and indicate if changes were made.
 * Full license: https://creativecommons.org/licenses/by-nc/4.0/legalcode
 */

#include "vulkan/vk_sync.hpp"
#include "vulkan/vk_context.hpp"

void createSyncObjects(VkContext& context)
{
	context.presentCompleteSemaphores.clear();
	context.renderFinishedSemaphores.clear();
	context.inFlightFences.clear();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		context.presentCompleteSemaphores.emplace_back(context.device.createSemaphore(vk::SemaphoreCreateInfo()));
		context.renderFinishedSemaphores.emplace_back(context.device.createSemaphore(vk::SemaphoreCreateInfo()));
		context.inFlightFences.emplace_back(context.device.createFence({ .flags = vk::FenceCreateFlagBits::eSignaled }));
	}
}
