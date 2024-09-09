#include "world.h"

void world::generateFlatLand()
{
	// Generate vertices
	for (int i = 0; i < worldSize.x * worldSize.y; i++) {
		vertexData.push_back(i);
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

world::world(int width, int height)
{
	this->worldSize.x = width;
	this->worldSize.y = height;
	generateFlatLand();
}

world::~world() {}