#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 uProjView;
uniform vec3 uPositionOffset;

void main()
{
    gl_Position = uProjView * vec4(position + uPositionOffset, 1.0);
}