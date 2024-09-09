#pragma once
#include <GL/glew.h>
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

class World
{
private:
	GLuint VBO, IBO;
	// World size, x = width, y = height
	glm::vec2 worldSize;

	// For now, each index carries a height value and thats it
	std::vector<glm::vec3> vertexData;

	// Indicies, make a triangle with verticies making a square
	std::vector<glm::vec3> indices;

public:
	void GenerateBuffers();
	void CompileShaders();
	void GenerateFlatLand();
	World(int width, int height);
	~World();
};