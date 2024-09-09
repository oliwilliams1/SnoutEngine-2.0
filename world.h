#pragma once
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

class world
{
private:
	// World size, x = width, y = height
	glm::vec2 worldSize;

	// For now, each index carries a height value and thats it
	std::vector<int> vertexData;

	// Indicies, make a triangle with verticies making a square
	std::vector<glm::vec3> indices;

public:
	void generateFlatLand();
	world(int width, int height);
	~world();
};