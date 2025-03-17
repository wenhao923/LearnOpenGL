#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in vec3 tangent;

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
   
   vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
   vec3 N = normalize(vec3(model * vec4(normal,    0.0)));
   vec3 B = cross(T, N);
   vs_out.TBN = mat3(T, B, N);
};