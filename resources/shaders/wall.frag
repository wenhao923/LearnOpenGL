#version 330 core
out vec4 FragColor; 

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D heightTexture;
uniform float height_scale;

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
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
vec2 myTexCoords;

void main()
{
	// �Ӳ���ͼ����texcoord(���߿ռ����)
	vec3 view = normalize(transpose(fs_in.TBN) * normalize(viewPos - fs_in.FragPos));
	myTexCoords = ParallaxMapping(fs_in.TexCoords, view);
	if(myTexCoords.x > 1.0 || myTexCoords.y > 1.0 || myTexCoords.x < 0.0 || myTexCoords.y < 0.0)
		discard;


	// ������ͼ�����Ŷ���ķ��߷���(����ռ�ķ���)
	vec3 Normal = vec3(texture(normalTexture, myTexCoords));
	Normal = normalize(2.0 * Normal - 1.0);
	Normal = normalize(fs_in.TBN * Normal);

	vec3 result = vec3(0.0);
	for (int i = 0; i < 1; i++) {
		result += CalcPointLight(pointLights[i], Normal, fs_in.FragPos, normalize(viewPos - fs_in.FragPos));
	}

    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	// ambient
    vec3 ambient = vec3(texture(diffuseTexture, myTexCoords)) * 0.1;
	// diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(diffuseTexture, myTexCoords));
	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(halfwayDir, normal), 0.0), 64);
	vec3 specular = spec * vec3(texture(diffuseTexture, myTexCoords));

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return ambient + diffuse + specular;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(heightTexture, texCoords).r;    
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;    
}