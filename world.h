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

class World {
private:
    GLuint VBO, normalVBO;
    GLuint ShaderProgram;
    GLuint uProjViewLocation;
    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> vertexNormals;

    glm::ivec2 worldSize;
    const char* pVSFileName = "shaders/world.vert";
    const char* pFSFileName = "shaders/world.frag";

    glm::vec2 gridSize;
	std::vector<glm::vec2> gridGradients;
    float PerlinNoise(glm::vec2 pos);
    float PerlinNoiseOctaves(glm::vec2 pos);

    Camera* camera;

    void CompileShaders();
    void GenerateBuffers();
    void GenerateLand();

    glm::vec2 RandomGradient(int ix, int iy, unsigned int seed);
    void GenerateGradients(unsigned int seed);

public:
    AABB aabb;
    World(int width, int height, Camera* camera, unsigned int seed);
    ~World();
    void Draw();
    void ChangeTerrainSeed(unsigned int seed);
};