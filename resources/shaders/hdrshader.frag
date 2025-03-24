#version 330 core
in vec2 TexCoords;

out vec4 FragColor; 

uniform sampler2D hdrMap;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{
    vec3 hdrColor = texture(hdrMap, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor; // additive blending

    // ÆØ¹âÉ«µ÷Ó³Éä
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    FragColor = vec4(mapped, 1.0);
}