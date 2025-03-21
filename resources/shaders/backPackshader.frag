#version 330 core

out vec4 FragColor; 

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    float shininess;
}; 

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform vec3 viewPos;
uniform samplerCube skybox;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() 
{	
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 采样反射贴图
    vec3 reflect_uv = normalize(reflect(viewDir, norm));
    vec3 reflect_color = texture(material.texture_diffuse2, TexCoords).rgb * texture(skybox, reflect_uv).rgb;

    // 折射光线
    float ratio = 1.00 / 1.52;
    vec3 refract_uv = normalize(refract(-viewDir, norm, ratio));
    vec3 refract_color = texture(skybox, refract_uv).rgb;

    vec3 result = texture(material.texture_diffuse1, TexCoords).rgb;
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        if (pointLights[i].constant != 0)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    } 
    
    //result += reflect_color;

    FragColor = vec4(result, 1.0);
};

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}