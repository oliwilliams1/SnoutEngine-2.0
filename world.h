#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

class World {
private:
    void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
    bool ReadFile(const char* pFileName, std::string& outFile);

    GLuint VBO = 0, IBO = 0;
    GLuint uProjViewLocation;
    std::vector<glm::vec3> vertexData;
    std::vector<GLuint> indices;
    glm::ivec2 worldSize;
    const char* pVSFileName = "shaders/world.vert";
    const char* pFSFileName = "shaders/world.frag";

    glm::vec2 gridSize;
	std::vector<glm::vec2> gridGradients;
    float PerlinNoise(glm::vec2 pos);
    float PerlinNoiseOctaves(glm::vec2 pos);

    Camera* camera;

public:
    World(int width, int height, Camera* camera, unsigned int seed);
    ~World();
    void CompileShaders();
    void GenerateBuffers();
    void GenerateFlatLand();
    void Draw();
    glm::vec2 RandomGradient(int ix, int iy, unsigned int seed);
    void GenerateGradients(unsigned int seed);
};