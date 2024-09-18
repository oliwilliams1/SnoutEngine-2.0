#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

struct CameraData {
    float width, height, near, far;
};

struct Ray {
    glm::vec3 pos;
    glm::vec3 direction;
};

struct AABB {
	glm::vec3 min;
	glm::vec3 max;
};

struct ClosestDistanceAndIndex {
    float distance;
	int index;
};

bool RayIntersectAABB(Ray ray, AABB aabb);

bool ReadFile(const char* pFileName, std::string& outFile);

void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);