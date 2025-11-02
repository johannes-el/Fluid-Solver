#pragma once

#include "vulkan/vk_context.hpp"
#include <glm/glm.hpp>

class Camera {
public:
	Camera(VkContext& context);
	glm::mat4 getViewMatrix();
	glm::mat4 getRotationMatrix();
	void processEvent();
	void update();
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	glm::vec3 velocity;
	glm::vec3 position;

	float pitch { 0.f };
	float yaw { 0.f };

private:
	const float cameraSpeed = 0.05f;

	VkContext& context;

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
};
