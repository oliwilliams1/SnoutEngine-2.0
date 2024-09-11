#version 330 core

layout(location = 0) in vec3 position;

out float height;

uniform mat4 uProjView;

void main() {
    height = position.y;
    gl_Position = uProjView * vec4(position, 1.0);
}