#version 330 core
in vec2 TexCoords;

out vec4 FragColor; 

struct Material {
    sampler2D texture_diffuse1;
    sampler2D specular;
    float shininess;
}; 
uniform Material material;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // 转换为 NDC
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{   
    FragColor = texture(material.texture_diffuse1, TexCoords);

    //float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
    //FragColor = vec4(vec3(depth), 1.0);
}