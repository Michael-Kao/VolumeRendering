#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aTexCoord;

out vec3 FragPos;
out vec3 TexCoord;

uniform mat4 transform;

void main()
{
    FragPos = aPosition;
    TexCoord = aTexCoord;
    gl_Position = transform * vec4(aPosition, 1.0);
}