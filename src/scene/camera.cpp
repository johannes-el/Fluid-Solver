#include "scene/camera.hpp"

void Camera::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
}

Camera::Camera(VkContext& context)
		: context(context)
{
	cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = -90.0f;
	pitch = 0.0f;

	// glfwSetCursorPosCallback(context.window, Camera::cursor_position_callback);
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::getRotationMatrix()
{
	glm::mat4 rotation = glm::mat4(1.0f);
	rotation = glm::rotate(rotation, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	// rotation = glm::rotate(rotation, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	// rotation = glm::rotate(rotation, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	return rotation;
}

void Camera::processEvent()
{
	if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (glfwGetKey(context.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		cameraPos += glm::vec3(0.0, 1.0, 0.0) * cameraSpeed;
	}
	if (glfwGetKey(context.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		cameraPos -= glm::vec3(0.0, 1.0, 0.0) * cameraSpeed;
	}
}


void Camera::update()
{

}
