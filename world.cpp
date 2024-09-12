#include "world.h"

// Generate a gradient from two random numbers, always the same number as psuedo-random
glm::vec2 World::RandomGradient(int ix, int iy, unsigned int seed) {
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = ix + seed, b = iy + seed; // Make seed acually do something
	a *= 3284157443;

	b ^= a << s | a >> w - s;
	b *= 1911520717;

	a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1));

	glm::vec2 v;
	v.x = sin(random);
	v.y = cos(random);

	return v;
}

// No idea how this works no matter how much I look at it
float World::PerlinNoise(glm::vec2 pos) {
	// Calculate grid cell coordinates
	int x0 = static_cast<int>(floor(pos.x));
	int y0 = static_cast<int>(floor(pos.y));
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	// Relative x and y in the unit square
	float sx = pos.x - x0;
	float sy = pos.y - y0;

	// Fade curve for smoothing
	float fadeX = sx * sx * sx * (sx * (sx * 6 - 15) + 10);
	float fadeY = sy * sy * sy * (sy * (sy * 6 - 15) + 10);

	// Hash coordinates to get gradient indices
	int ix0 = (int)(x0 + y0 * gridSize.x) % gridGradients.size();
	int ix1 = (int)(x1 + y0 * gridSize.x) % gridGradients.size();
	int iy0 = (int)(x0 + y1 * gridSize.x) % gridGradients.size();
	int iy1 = (int)(x1 + y1 * gridSize.x) % gridGradients.size();

	// Dot products from corneres of cell
	float grad00 = glm::dot(gridGradients[ix0], glm::vec2(sx, sy));
	float grad10 = glm::dot(gridGradients[ix1], glm::vec2(sx - 1, sy));
	float grad01 = glm::dot(gridGradients[iy0], glm::vec2(sx, sy - 1));
	float grad11 = glm::dot(gridGradients[iy1], glm::vec2(sx - 1, sy - 1));

	// Interpolate along x
	float lerpX0 = grad00 + fadeX * (grad10 - grad00);
	float lerpX1 = grad01 + fadeX * (grad11 - grad01);

	// Interpolate along y
	return lerpX0 + fadeY * (lerpX1 - lerpX0);
}

float World::PerlinNoiseOctaves(glm::vec2 pos) {
	float total = 0.0f;
	float frequency = 1.0f;
	float amplitude = 1.0f;
	float maxValue = 0.0f;

	for (int i = 0; i < 6; i++) {
		total += PerlinNoise(pos * frequency) * amplitude;
		maxValue += amplitude;

		amplitude *= 0.5f;
		frequency *= 2.0f;
	}

	return (total / maxValue) * 5.0f;
}

// Populate the gridGradients vector for later use
void World::GenerateGradients(unsigned int seed) {
	for (int y = 0; y < gridSize.y; y++) {
		for (int x = 0; x < gridSize.x; x++) {
			gridGradients.push_back(RandomGradient(x, y, seed));
		}
	}
}

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
	// Make VBO for vertex positions
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexPositions.size() * sizeof(glm::vec3), vertexPositions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	// Make VBO for face normals
	GLuint normalVBO;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexNormals.size() * sizeof(glm::vec3), vertexNormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
}

void World::GenerateFlatLand()
{
	// Ensure a clear incase multi-generation
	vertexPositions.clear();
	vertexNormals.clear();

	// Generate vertices and normals
	for (int y = 0; y < worldSize.y; y++) {
		for (int x = 0; x < worldSize.x; x++) {
			glm::vec2 v1(x, y);
			glm::vec2 v2(x + 1, y);
			glm::vec2 v3(x, y + 1);

			glm::vec3 v1_3d(v1.x, PerlinNoiseOctaves(v1 / gridSize), v1.y);
			glm::vec3 v2_3d(v2.x, PerlinNoiseOctaves(v2 / gridSize), v2.y);
			glm::vec3 v3_3d(v3.x, PerlinNoiseOctaves(v3 / gridSize), v3.y);

			glm::vec3 f1Normal = -glm::normalize(glm::cross(v2_3d - v1_3d, v3_3d - v1_3d));

			// Add vertex data for 1st tri
			vertexPositions.push_back(v1_3d);
			vertexPositions.push_back(v2_3d);
			vertexPositions.push_back(v3_3d);

			vertexNormals.push_back(f1Normal);
			vertexNormals.push_back(f1Normal);
			vertexNormals.push_back(f1Normal);

			glm::vec2 v4(x + 1, y);
			glm::vec2 v5(x + 1, y + 1);
			glm::vec2 v6(x, y + 1);

			glm::vec3 v4_3d(v4.x, PerlinNoiseOctaves(v4 / gridSize), v4.y);
			glm::vec3 v5_3d(v5.x, PerlinNoiseOctaves(v5 / gridSize), v5.y);
			glm::vec3 v6_3d(v6.x, PerlinNoiseOctaves(v6 / gridSize), v6.y);

			glm::vec3 f2Normal = -glm::normalize(glm::cross(v5_3d - v4_3d, v6_3d - v4_3d));

			// Add vertex data for 2nd tri
			vertexPositions.push_back(v4_3d);
			vertexPositions.push_back(v5_3d);
			vertexPositions.push_back(v6_3d);

			vertexNormals.push_back(f2Normal);
			vertexNormals.push_back(f2Normal);
			vertexNormals.push_back(f2Normal);
		}
	}

	// Offset vertices so grid is centered
	for (int i = 0; i < vertexPositions.size(); i++) {
		vertexPositions[i] += glm::vec3(-worldSize.x / 2.0f, 0.0f, -worldSize.y / 2.0f);
	}	
}

void World::Draw()
{
	// Set uniforms
	glUniformMatrix4fv(uProjViewLocation, 1, GL_FALSE, glm::value_ptr(camera->projView));

	// Bind buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Attribs
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Draw using glDrawArrays
	glDrawArrays(GL_TRIANGLES, 0, vertexPositions.size());

	// Clean up
	glDisableVertexAttribArray(0);
}

World::World(int width, int height, Camera* camera, unsigned int seed)
{
	// Init variables
	this->worldSize.x = width;
	this->worldSize.y = height;
	this->camera = camera;

	
	int maxSize = (height > width) ? height : width;
	gridSize = glm::vec2(maxSize, maxSize);
	GenerateGradients(seed);

	GenerateFlatLand();
	GenerateBuffers();

	// Compile shaders AFTER generating buffers
	CompileShaders();
}

World::~World() {}