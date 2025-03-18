#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
   vs_out.TexCoords = aTexcoord;
   
   vec3 T = normalize(mat3(model) * tangent);
   vec3 B = normalize(mat3(model) * bitangent);
   vec3 N = normalize(mat3(model) * normal);
   vs_out.TBN = mat3(T, B, N);
};