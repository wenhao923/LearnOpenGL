#version 330 core
in vec2 TexCoords;
in vec3 Normal; 
in vec3 FragPos;

out vec4 FragColor; 

struct Material {
	sampler2D emit;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
uniform Material material;

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

uniform vec3 cameraPos;

void main() 
{	
	// emit
	vec3 emit = vec3(texture(material.emit, TexCoords));
	// ambient
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	// specular
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	vec3 result = emit + ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
};