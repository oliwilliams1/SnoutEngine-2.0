#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

GLFWwindow* createWindow(int width, int height, const char* title) {
    glfwSetErrorCallback(error_callback);

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
    GLFWwindow* window = createWindow(800, 600, "OpenGL Window");
    if (!window) return -1;

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}