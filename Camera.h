#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "utils.h"

class Camera {
private:
    GLFWwindow* window;

    double* deltaTime;
    float speed = 10.0f;
    bool mouseDown = false;

    glm::mat4 view;
    glm::mat4 proj;

    void UpdateProj();
    void AdjustCameraSize(float delta, bool increase);


public:
    CameraData cameraData;
    CameraData initCameraData;

    glm::vec3 position = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 viewDir = glm::vec3(-1.0f, -0.75f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 target = position + viewDir;

    AABB* worldAABB;
    glm::mat4 projView;
    glm::vec3* spherePos;
    Camera(GLFWwindow* window, double* deltaTime, float width, float height, float near = -1000.0f, float far = 1000.0f, glm::vec3* spherePos = nullptr);

    ~Camera();
    void Update();
};