#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

    // Set opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    Camera camera(window, &deltaTime, 24.0f, 24.0f);

    int seed = 1;

    // World, 16x16 grid, pass camera for shaders
    World world(32, 32, &camera, seed);

    // Escape key closes window
    glfwSetKeyCallback(window, KeyCallback);

    // Render as wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Imgui stuff
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Delta time
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Update camera
        camera.update();

        // Draw world
        world.Draw();

        ImGui::Begin("Seed changer");
        ImGui::DragInt("Seed", &seed);
        if (ImGui::Button("Button")) {
            world.ChangeTerrainSeed((unsigned int)abs(seed));
        }
		ImGui::End();

        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}