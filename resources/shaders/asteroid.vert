﻿#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
   gl_Position = projection * view * instanceMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   TexCoords = aTexcoord;
};