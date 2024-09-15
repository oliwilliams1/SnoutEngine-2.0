#include "Sphere.h"

void Sphere::CompileShaders()
{
    ShaderProgram = glCreateProgram();
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

    uPositionOffsetLocation = glGetUniformLocation(ShaderProgram, "uPositionOffset");
    if (uPositionOffsetLocation == -1) {
        printf("Error getting uniform location of 'uPositionOffset'\n");
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

// Chatgpt makes spheres
void Sphere::GenerateSphere() {
    float radius = 0.5f;
    int sectors = 12;
    int stacks = 12;

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI / 2 - i * stackStep; // Starting from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle); // Radius at current stack
        float z = radius * sinf(stackAngle);  // Height at current stack

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep; // Current sector angle
            float x = xy * cosf(sectorAngle); // X position
            float y = xy * sinf(sectorAngle); // Y position
            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    // Generate indices for the triangles
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = (i * (sectors + 1)) + j;
            int second = first + sectors + 1;

            // First triangle
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            // Second triangle
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    // Offset sphere vertices
	for (int i = 0; i < vertices.size(); ++i) {
		vertices[i] += glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

void Sphere::GenerateBuffers() {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
}

void Sphere::Draw() {
    glUseProgram(ShaderProgram);

    // Set uniforms
    glUniformMatrix4fv(uProjViewLocation, 1, GL_FALSE, glm::value_ptr(camera->projView));
    glUniform3f(uPositionOffsetLocation, position.x, position.y, position.z);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    // Clean up
    glDisableVertexAttribArray(0);
}

Sphere::Sphere(Camera* camera)
{
    this->camera = camera;

    GenerateSphere();
    GenerateBuffers();
	CompileShaders();
}

Sphere::~Sphere()
{
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(ShaderProgram);
}