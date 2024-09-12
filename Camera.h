#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraData {
    float width, height, near, far;
};

class Camera {
private:
    GLFWwindow* window;
    double* deltaTime;
    float speed = 5.0f;
    CameraData cameraData;

    glm::vec3 position = glm::vec3(5.0f, 3.0f, 5.0f);
    glm::vec3 viewDir = glm::vec3(-1.0f, -0.6f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 target = position + viewDir;

    glm::mat4 view;
    glm::mat4 proj;

    void updateProj();

public:
    glm::mat4 projView;

    Camera(GLFWwindow* window, double* deltaTime, float width, float height, float near = -1000.0f, float far = 1000.0f);

    ~Camera();
    void update();
};