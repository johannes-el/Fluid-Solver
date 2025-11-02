#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "vulkan/vk_context.hpp"
#include <glm/glm.hpp>

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;

	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec3 viewPos;
	alignas(16) glm::vec3 lightColor;
	alignas(16) glm::vec3 objectColor;

	static float Pos;
	static float Scale;

	static void updateUniformBuffer(VkContext& context, uint32_t currentImage);
};
