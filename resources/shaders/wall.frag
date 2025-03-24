#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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
	// 视差贴图计算texcoord(切线空间计算)
	vec3 view = normalize(transpose(fs_in.TBN) * normalize(viewPos - fs_in.FragPos));
	myTexCoords = ParallaxMapping(fs_in.TexCoords, view);
	if(myTexCoords.x > 1.0 || myTexCoords.y > 1.0 || myTexCoords.x < 0.0 || myTexCoords.y < 0.0)
		discard;


	// 法线贴图计算扰动后的法线方向(世界空间的法线)
	vec3 Normal = vec3(texture(normalTexture, myTexCoords));
	Normal = normalize(2.0 * Normal - 1.0);
	Normal = normalize(fs_in.TBN * Normal);

	vec3 result = vec3(0.0);
	for (int i = 0; i < 1; i++) {
		result += CalcPointLight(pointLights[i], Normal, fs_in.FragPos, normalize(viewPos - fs_in.FragPos));
	}

    FragColor = vec4(result, 1.0);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
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
    float height_scale = 0.1;
    const float minLayers = 8.0, maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(viewDir.z));
    float layerDepth = 1.0 / numLayers;
    
	vec2 P = viewDir.xy * height_scale; 
    vec2 deltaUV = P / numLayers;

    float currentLayerDepth = 0.0;
    vec2 currentUV = texCoords;
    float currentDepth = texture(heightTexture, currentUV).r;
    
    // 分层步进逼近真实交点
    while (currentLayerDepth < currentDepth) {
        currentUV -= deltaUV; // 分层应用偏移
        currentDepth = texture(heightTexture, currentUV).r;
        currentLayerDepth += layerDepth;
    }

	vec2 prevTexCoords = currentUV + deltaUV;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepth - currentLayerDepth;
	float beforeDepth = texture(heightTexture, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = mix(currentUV, prevTexCoords, weight);

	return finalTexCoords;
}