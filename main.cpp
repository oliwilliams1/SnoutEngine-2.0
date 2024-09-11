#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"
#include "World.h"

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void Error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

GLFWwindow* CreateWindow(int width, int height, const char* title) {
    glfwSetErrorCallback(Error_callback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    return window;
}

int main() {
    GLFWwindow* window = CreateWindow(600, 600, "OpenGL Window");
    if (!window) return -1;

    // Initialize GLEW after creating the OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Time variables
    double lastTime = glfwGetTime();
    double deltaTime;

    // Camera object, ortho camera 16x16
    Camera camera(window, &deltaTime, 16.0f, 16.0f);

    // World, 16x16 grid, pass camera for shaders
    World world(16, 16, &camera, 2135);

    // Escape key closes window
    glfwSetKeyCallback(window, KeyCallback);

    // Render as wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Delta time
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Update camera
        camera.update();

        // Draw world
        world.Draw();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Destroy
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}