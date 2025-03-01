#version 330 core
in vec3 Normal; 
in vec3 FragPos;

out vec4 FragColor; 

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main() 
{	
	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + spec) * objectColor;
	FragColor = vec4(result, 1.0);
};