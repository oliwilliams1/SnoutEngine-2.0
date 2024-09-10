#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    glm::mat4 view;
    glm::mat4 proj;

public:
    glm::mat4 projView;

    Camera(float width, float height, float near = -1000.0f, float far = 1000.0f) {
        proj = glm::ortho(-width, width, -height, height, near, far);
        updateView();
    }

    ~Camera() {}

    void updateView() {

        glm::vec3 position = glm::vec3(5.0f, 5.0f, 5.0f);
        glm::vec3 viewDir = glm::vec3(-1.0f, -1.0f, -1.0f);
        glm::vec3 target = position + viewDir;
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        view = glm::lookAt(position, target, up);

        projView = proj * view;
    }
};