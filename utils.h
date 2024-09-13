#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <GL/gl.h>

bool ReadFile(const char* pFileName, std::string& outFile);

void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);