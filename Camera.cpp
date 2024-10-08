#include "Camera.h"

void Camera::UpdateProj() {
	proj = glm::ortho(
		-cameraData.width,
		cameraData.width,
		-cameraData.height,
		cameraData.height,
		cameraData.near,
		cameraData.far
	);
}

void Camera::AdjustCameraSize(float delta, bool increase) {
	float adjustment = increase ? delta * cameraData.width : -delta * cameraData.width;

	cameraData.width += adjustment;
	cameraData.height += adjustment;

	// Ensure width and height do not exceed initial limits or fall below minimum
	cameraData.width = glm::clamp(cameraData.width, 10.0f, initCameraData.width);
	cameraData.height = glm::clamp(cameraData.height, 10.0f, initCameraData.height);

	UpdateProj();
}

void Camera::Update() {
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {
		speed = 10.0f;
	}
	else {
		speed = 20.0f;
	}

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
		AdjustCameraSize(*deltaTime, true);
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		AdjustCameraSize(*deltaTime, false);
	}

	target = position + viewDir;

	view = glm::lookAt(position, target, up);
	projView = proj * view;
}

Camera::Camera(GLFWwindow* window, double* deltaTime, float width, float height, float near, float far, glm::vec3* spherePos) {
	this->window = window;
	this->deltaTime = deltaTime;
	this->cameraData = { width, height, near, far };
	this->initCameraData = this->cameraData;
	this->spherePos = spherePos;
	UpdateProj();
	Update();
}

Camera::~Camera() {};