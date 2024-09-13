#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 uProjView;

void main()
{
    gl_Position = uProjView * vec4(position, 1.0);
}