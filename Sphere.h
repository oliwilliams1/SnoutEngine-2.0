#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "utils.h"
#define M_PI 3.14159265358979323846

class Sphere
{
private:
	GLuint VBO, IBO;
	GLuint ShaderProgram;
	GLuint uProjViewLocation, uPositionOffsetLocation;
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

	const char* pVSFileName = "shaders/sphere.vert";
	const char* pFSFileName = "shaders/sphere.frag";

	Camera* camera;

	void CompileShaders();
	void GenerateBuffers();
	void GenerateSphere();

	glm::vec3* spherePos;

public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	void Draw();
	Sphere(Camera* camera, glm::vec3* spherePos);
	~Sphere();	
};