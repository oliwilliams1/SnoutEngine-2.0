#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 fragNormal;
out float height;

uniform mat4 uProjView;

void main()
{
    gl_Position = uProjView * vec4(position, 1.0);
    fragNormal = normal;
    height = position.y;
}