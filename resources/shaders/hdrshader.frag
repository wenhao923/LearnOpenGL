#version 330 core
in vec2 TexCoords;

out vec4 FragColor; 

uniform sampler2D texture_diffuse1;
uniform float exposure;

void main()
{
    vec3 hdrColor = texture(texture_diffuse1, TexCoords).rgb;

    // ÆØ¹âÉ«µ÷Ó³Éä
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    FragColor = vec4(mapped, 1.0);
}