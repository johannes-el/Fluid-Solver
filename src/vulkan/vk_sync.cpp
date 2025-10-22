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

	context.renderFinishedSemaphores.resize(context.swapChainImages.size());
	context.presentCompleteSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	context.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	vk::SemaphoreCreateInfo semaphoreInfo{};
	vk::FenceCreateInfo fenceInfo{};
	fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		context.presentCompleteSemaphores[i] = context.device.createSemaphore(semaphoreInfo);
		context.inFlightFences[i] = context.device.createFence(fenceInfo);
	}

	for (size_t i = 0; i < context.swapChainImages.size(); ++i) {
		context.renderFinishedSemaphores[i] = context.device.createSemaphore(semaphoreInfo);
	}
}
