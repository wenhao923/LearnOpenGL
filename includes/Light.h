// Light.h
#pragma once
#include <glm/glm.hpp>

enum LightType
{
	Point,
	Directional,
	Spot
};

class Light {
public:
    virtual ~Light() = default;

    // 设置光源颜色
    void SetColor(const glm::vec3& color) { this->color = color; }
    glm::vec3 GetColor() const { return color; }

    // 设置光源强度
    void SetIntensity(float intensity) { this->intensity = intensity; }
    float GetIntensity() const { return intensity; }

    //// 更新光源属性（子类可重写）
    //virtual void Update(float deltaTime) {}

	virtual LightType GetType() const = 0;

public:
    glm::vec3 color = glm::vec3(1.0f); // 光源颜色
    float intensity = 1.0f;            // 光源强度
};