#include "world.h"

void World::CompileShaders()
{

}

void World::GenerateBuffers()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(int), vertexData.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::vec3), indices.data(), GL_STATIC_DRAW);
}

void World::GenerateFlatLand()
{
	// Generate vertices
	for (int y = 0; y < worldSize.y; y++) {
		for (int x = 0; x < worldSize.x; x++) {
			vertexData.push_back(glm::vec3(x, 1.0f, y));
		}
	}

	// Generate indicies - threory from somewhere online
    for (int y = 0; y < worldSize.y - 1; y++) {
        for (int x = 0; x < worldSize.x - 1; x++) 
		{
			int v1 = y * worldSize.x + x;
			int v2 = y * worldSize.x + x + 1;
			int v3 = (y + 1) * worldSize.x + x;
			int v4 = (y + 1) * worldSize.x + x + 1;
			indices.push_back(glm::vec3(v1, v2, v4));
			indices.push_back(glm::vec3(v1, v4, v3));
        }
    }
}

World::World(int width, int height)
{
	worldSize.x = width;
	worldSize.y = height;
	GenerateFlatLand();
	GenerateBuffers();
}

World::~World() {}