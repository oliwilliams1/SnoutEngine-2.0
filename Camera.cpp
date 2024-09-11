#include "Camera.h"

void Camera::update() {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position.x -= *deltaTime * speed;
		position.z -= *deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position.x += *deltaTime * speed;
		position.z += *deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position.x += *deltaTime * speed;
		position.z -= *deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position.x -= *deltaTime * speed;
		position.z += *deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
		cameraData.width  += *deltaTime * speed;
		cameraData.height += *deltaTime * speed;
		updateProj();
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		cameraData.width  -= *deltaTime * speed;
		cameraData.height -= *deltaTime * speed;
		updateProj();
	}
	target = position + viewDir;

	view = glm::lookAt(position, target, up);
	projView = proj * view;
}