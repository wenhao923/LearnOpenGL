#version 330 core
in vec2 TexCoords;

out vec4 FragColor; 

uniform sampler2D texture_diffuse1;

void main()
{   
    vec3 col = texture(texture_diffuse1, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
}