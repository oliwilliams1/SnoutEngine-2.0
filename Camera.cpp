#include "Camera.h"

void Camera::updateProj() {
	proj = glm::ortho(
		-cameraData.width,
		cameraData.width,
		-cameraData.height,
		cameraData.height,
		cameraData.near,
		cameraData.far
	);
}

void Camera::adjustCameraSize(float delta, bool increase) {
	float adjustment = increase ? delta * cameraData.width : -delta * cameraData.width;

	cameraData.width += adjustment;
	cameraData.height += adjustment;

	// Ensure width and height do not exceed initial limits or fall below minimum
	cameraData.width = glm::clamp(cameraData.width, 10.0f, initCameraData.width);
	cameraData.height = glm::clamp(cameraData.height, 10.0f, initCameraData.height);

	updateProj();
}

void Camera::calculateMousePress() {
	Ray ray;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Normalize mouse position to NDC
	float ndcX = (2.0f * xpos) / width - 1.0f; // Normalize to [-1, 1]
	float ndcY = 1.0f - (2.0f * ypos) / height; // Normalize to [1, -1]

	glm::vec3 up = glm::normalize(viewDir);
	glm::vec3 right = glm::normalize(glm::cross(up, this->up));
	if (glm::length(right) < 0.0001f) {
		right = glm::normalize(glm::cross(up, glm::vec3(1.0f, 0.0f, 0.0f)));
	}
	glm::vec3 forward = glm::cross(up, right);

	glm::vec2 offsets;
	offsets.x = (ndcX) * cameraData.width;
	offsets.y = -ndcY * cameraData.height;

	// Position on the plane
	ray.pos = this->position + right * offsets.x + forward * offsets.y;
	ray.direction = this->viewDir;

	float t = -ray.pos.y / ray.direction.y;

	glm::vec2 point;
	point.x = ray.pos.x + t * ray.direction.x;
	point.y = ray.pos.z + t * ray.direction.z;
	std::cout << point.x << " " << point.y << std::endl;
	spherePos->x = point.x;
	spherePos->z = point.y;
}

void Camera::update() {
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
		adjustCameraSize(*deltaTime, true);
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		adjustCameraSize(*deltaTime, false);
	}

	target = position + viewDir;

	view = glm::lookAt(position, target, up);
	projView = proj * view;

	switch (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
		case GLFW_PRESS:
			mouseDown = true;
			break;

		case GLFW_RELEASE:
			if (mouseDown) {
				calculateMousePress();
			}
			mouseDown = false;
			break;
	}
}

Camera::Camera(GLFWwindow* window, double* deltaTime, float width, float height, float near, float far, glm::vec3* spherePos) {
	this->window = window;
	this->deltaTime = deltaTime;
	this->cameraData = { width, height, near, far };
	this->initCameraData = this->cameraData;
	this->spherePos = spherePos;
	updateProj();
	update();
}

Camera::~Camera() {};