#version 330 core
in vec2 TexCoords;
in vec3 FragPos;

out vec4 FragColor; 

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 Normal = vec3(texture(normalTexture, TexCoords));
	Normal = 2.0*Normal - 1.0;

	vec3 result = vec3(0.0);
	for (int i = 0; i < 1; i++) {
		result += CalcPointLight(pointLights[i], Normal, FragPos, normalize(viewPos - FragPos));
	}

    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	// ambient
    vec3 ambient = vec3(texture(diffuseTexture, TexCoords)) * light.ambient*0.1;
	// diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, TexCoords));
	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = light.specular * spec * vec3(texture(diffuseTexture, TexCoords));

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return ambient + diffuse + specular;
}
