#include <iostream>
#include <fstream>
#include <string>

#include "world.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void World::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
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

bool World::ReadFile(const char* pFileName, std::string& outFile)
{
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

void World::CompileShaders() {
    GLuint ShaderProgram = glCreateProgram();
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    std::string vs, fs;

    if (!ReadFile(pVSFileName, vs)) {
        exit(1);
    }
    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

    if (!ReadFile(pFSFileName, fs)) {
        exit(1);
    }
    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);

	// Uniforms
	uProjViewLocation = glGetUniformLocation(ShaderProgram, "uProjView");
	if (uProjViewLocation == -1) {
		printf("Error getting uniform location of 'uProjView'\n");
		exit(1);
	}

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);
}

void World::GenerateBuffers() 
{
	// Make VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(glm::vec3), vertexData.data(), GL_STATIC_DRAW);

	// Pos of vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	// Make IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void World::GenerateFlatLand()
{
	// Generate vertices
	for (int y = 0; y < worldSize.y; y++) {
		for (int x = 0; x < worldSize.x; x++) {
			vertexData.push_back(glm::vec3(x, 0.0f, y));
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
			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v4);
			indices.push_back(v1);
			indices.push_back(v4);
			indices.push_back(v3);
		}
	}

	// Offset verticies so grid is centered
	for (int i = 0; i < vertexData.size(); i++) {
		vertexData[i] += glm::vec3(-worldSize.x / 2.0f, 0.0f, -worldSize.y / 2.0f);
	}
}

void World::Draw()
{
	// Set uniforms
	glUniformMatrix4fv(uProjViewLocation, 1, GL_FALSE, glm::value_ptr(camera->projView));

	// Buffer stuff
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Attribs
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Draw
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	
	// Clean up
	glDisableVertexAttribArray(0);
}

World::World(int width, int height, Camera* camera)
{
	// Init variables
	this->worldSize.x = width;
	this->worldSize.y = height;
	this->camera = camera;

	GenerateFlatLand();
	GenerateBuffers();

	// Compile shaders AFTER generating buffers
	CompileShaders();
}

World::~World() {}