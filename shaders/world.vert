#version 330 core

layout(location = 0) in vec3 position;

mat4 isometricProjection = mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0
);

void main() {
    gl_Position = isometricProjection * vec4(position, 1.0);
}