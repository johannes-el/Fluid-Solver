#pragma once

#include "vk_context.hpp"

#ifdef NDEBUG
inline constexpr bool enableValidationLayers = false;
#else
inline constexpr bool enableValidationLayers = true;
#endif

void createSurface(VkContext& context);

void createInstance(VkContext& context);
void setupValidationLayers(VkContext& vkContext);
