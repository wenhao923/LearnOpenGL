#version 330 core
in vec2 TexCoords;

out vec4 FragColor; 

uniform sampler2D texture_diffuse1;

uniform sampler2D screenTexture;
uniform float screenWidth;
uniform float screenHeight;

void main()
{
    float offsetX = 1.0 / screenWidth;
    float offsetY = 1.0 / screenHeight;

    vec2 offsets[9] = vec2[](
        vec2(-offsetX,  offsetY), // 左上
        vec2( 0.0f,     offsetY), // 正上
        vec2( offsetX,  offsetY), // 右上
        vec2(-offsetX,  0.0f),    // 左
        vec2( 0.0f,     0.0f),    // 中
        vec2( offsetX,  0.0f),    // 右
        vec2(-offsetX, -offsetY), // 左下
        vec2( 0.0f,    -offsetY), // 正下
        vec2( offsetX, -offsetY)  // 右下
    );

    float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
}