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

    Camera* camera;

public:
    World(int width, int height, Camera* camera);
    ~World();
    void CompileShaders();
    void GenerateBuffers();
    void GenerateFlatLand();
    void Draw();
};