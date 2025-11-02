#include "vulkan/vk_context.hpp"
#include "scene/uniforms.hpp"
#include "scene/camera.hpp"

float UniformBufferObject::Pos = 0.0;
float UniformBufferObject::Scale = 0.5;

void UniformBufferObject::updateUniformBuffer(VkContext& context, uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	static Camera cam{context};
	cam.processEvent();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};

	float aspect = static_cast<float>(context.swapChainExtent.width) / static_cast<float>(context.swapChainExtent.height);

	glm::mat4 model = glm::mat4(1.0f);
	model = cam.getRotationMatrix();
	model = glm::scale(model, glm::vec3(0.05f) * Scale);

	// context.imGui

	ubo.model = model;
	ubo.model = glm::rotate(model, time * glm::radians(90.f), glm::vec3(0.0, 1.0, 0.0));
	ubo.view = cam.getViewMatrix();

	ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
	ubo.proj[1][1] *= -1;

	ubo.lightPos = glm::vec3(0.0, 10.0, 0.0);
	ubo.lightColor = glm::vec3(1.0, 1.0, 1.0);
	ubo.objectColor = glm::vec3(1.0, 0.0, 0.0);

	memcpy(context.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
