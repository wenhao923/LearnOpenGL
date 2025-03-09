#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 Normal;
} gs_in[];

const float MAGNITUDE = 0.1;

uniform mat4 projection;

void main()
{
    for (int i = 0; i < 3; i++) {
        gl_Position = projection * gl_in[i].gl_Position;
        EmitVertex();

        gl_Position = projection * (gl_in[i].gl_Position + MAGNITUDE * vec4(gs_in[i].Normal, 0.0) * 0.5);
        EmitVertex();
        EndPrimitive();
    }
};