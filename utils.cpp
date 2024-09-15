#include "utils.h"

bool RayIntersectAABB(Ray ray, AABB aabb)
{
    glm::vec3 invDir = 1.0f / ray.direction;
    glm::vec3 t0 = (aabb.min - ray.pos) * invDir;
    glm::vec3 t1 = (aabb.max - ray.pos) * invDir;

    glm::vec3 tMin = glm::min(t0, t1);
    glm::vec3 tMax = glm::max(t0, t1);

    float tNear = glm::max(tMin.x, glm::max(tMin.y, tMin.z));
    float tFar = glm::min(tMax.x, glm::min(tMax.y, tMax.z));

    return tNear <= tFar && tFar >= 0;
}

bool ReadFile(const char* pFileName, std::string& outFile) {
    std::ifstream f(pFileName);

    bool ret = false;

    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            outFile.append(line);
            outFile.append("\n");
        }

        f.close();
        ret = true;
    }
    else {
        std::cout << "Unable to open file: " << pFileName << std::endl;
    }

    return ret;
}

void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(1);
    }

    const GLchar* p[1];
    p[0] = pShaderText;

    GLint Lengths[1];
    Lengths[0] = (GLint)strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}