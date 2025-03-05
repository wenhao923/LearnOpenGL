#version 330 core
in vec2 TexCoords;

out vec4 FragColor; 

struct Material {
    sampler2D texture_diffuse1;
    sampler2D specular;
    float shininess;
}; 
uniform Material material;

void main() 
{	
	FragColor = vec4(texture(material.texture_diffuse1, TexCoords));
};