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

	return round((total / maxValue) * 7.5f) / 4.5f;
}

// Populate the gridGradients vector for later use
void World::GenerateGradients(unsigned int seed) {
	gridGradients.clear();
	for (int y = 0; y < gridSize.y; y++) {
		for (int x = 0; x < gridSize.x; x++) {
			gridGradients.push_back(RandomGradient(x, y, seed));
		}
	}
}

void World::CompileShaders() {
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
	glBufferData(GL_ARRAY_BUFFER, vertexPositions.size() * sizeof(glm::vec3), vertexPositions.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	// Make VBO for face normals
	GLuint normalVBO;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexNormals.size() * sizeof(glm::vec3), vertexNormals.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
}

// Generates land based off of grid gradients
void World::GenerateLand() {
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
	
	// Calculate AABB
	glm::vec3 min(0.0f);
	glm::vec3 max(0.0f);
	for (int i = 0; i < vertexPositions.size(); i++) {
		min = glm::min(min, vertexPositions[i]);
		max = glm::max(max, vertexPositions[i]);
	}
	this->aabb = { min, max };
}

void World::CalculateMousePress()
{
	Ray ray;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Normalize mouse position to NDC
	float ndcX = (2.0f * xpos) / width - 1.0f; // Normalize to [-1, 1]
	float ndcY = 1.0f - (2.0f * ypos) / height; // Normalize to [1, -1]

	glm::vec3 up = glm::normalize(camera->viewDir);
	glm::vec3 right = glm::normalize(glm::cross(up, camera->up));
	if (glm::length(right) < 0.0001f) {
		right = glm::normalize(glm::cross(up, glm::vec3(1.0f, 0.0f, 0.0f)));
	}
	glm::vec3 forward = glm::cross(up, right);

	glm::vec2 offsets;
	offsets.x =  ndcX * camera->cameraData.width;
	offsets.y = -ndcY * camera->cameraData.height;

	// Position on the plane
	ray.pos = camera->position + right * offsets.x + forward * offsets.y;
	ray.direction = camera->viewDir;

	float t = -ray.pos.y / ray.direction.y;

	glm::vec2 point;
	point.x = ray.pos.x + t * ray.direction.x;
	point.y = ray.pos.z + t * ray.direction.z;

	// Get [-0.5, 0.5]
	glm::vec2 posNDC = point / glm::vec2(worldSize.x, worldSize.y);
	posNDC += 0.5f;

	// Get [0, 0 -> max_width, max_height]
	posNDC *= worldSize;

	// Find index of the vertex its probably closest to
	// First of all round because yeah
	posNDC = { round(posNDC.x), round(posNDC.y) };

	if (0 < posNDC.x && posNDC.x < worldSize.x && 0 < posNDC.y && posNDC.y < worldSize.y) {

		std::vector<ClosestDistanceAndIndex> points;

		for (int y = -3; y <= 3; y++) {
			for (int x = -1; x <= 5; x++) {
				int index = (((int)posNDC.y + y) * 6 * worldSize.y) + ((int)posNDC.x + x) * 6 - 1;

				if (index >= 0 && index <= vertexPositions.size()) {
					glm::vec3 unitDir = glm::normalize(ray.direction);
					glm::vec3 toPoint = vertexPositions[index] - ray.pos;
					glm::vec3 vertexInQuestion = vertexPositions[index];
					float t = glm::dot(toPoint, unitDir);

					float distance;
					if (t < 0) {
						distance = glm::length(ray.pos - vertexInQuestion);
					}
					else {
						glm::vec3 closestPoint = ray.pos + t * unitDir;
						distance = glm::length(closestPoint - vertexInQuestion);
					}

					points.push_back({ distance, index });
				}
			}
		}

		int index = -1;
		float minDistance = std::numeric_limits<float>::max();

		for (const auto& point : points) {
			if (point.distance < minDistance) {
				minDistance = point.distance;
				index = point.index;
			}
		}

		sphere->position = vertexPositions[index];
	}
}

// Update the world, if needed
void World::Update() {
	switch (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
	case GLFW_PRESS:
		mouseDown = true;
		break;

	case GLFW_RELEASE:
		if (mouseDown) {
			CalculateMousePress();
		}
		mouseDown = false;
		break;
	}
}

// Main draw function, calls the shader and draws the world
void World::Draw() {
	// Update the world, if needed
	Update();

	// Use shader program
	glUseProgram(ShaderProgram);

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

	// Render sphere
	sphere->Draw();
}

// Change terrain seed, update buffers
void World::ChangeTerrainSeed(unsigned int seed) {
	GenerateGradients(seed);
	GenerateLand();
	GenerateBuffers();
}

World::World(int width, int height, GLFWwindow* window, Camera* camera, unsigned int seed) {
	// Init variables
	this->worldSize.x = width;
	this->worldSize.y = height;
	this->camera = camera;
	this->window = window;
	
	int maxSize = (height > width) ? height : width;
	gridSize = glm::vec2(maxSize, maxSize);
	GenerateGradients(seed);

	GenerateLand();
	GenerateBuffers();

	// Compile shaders AFTER generating buffers
	CompileShaders();

	sphere = new Sphere(camera);
}

// Prepare the world for a mass-bombing
World::~World() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteProgram(ShaderProgram);
}