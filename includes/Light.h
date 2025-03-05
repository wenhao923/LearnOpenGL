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

    // ���ù�Դ��ɫ
    void SetColor(const glm::vec3& color) { this->color = color; }
    glm::vec3 GetColor() const { return color; }

    // ���ù�Դǿ��
    void SetIntensity(float intensity) { this->intensity = intensity; }
    float GetIntensity() const { return intensity; }

    //// ���¹�Դ���ԣ��������д��
    //virtual void Update(float deltaTime) {}

	virtual LightType GetType() const = 0;

public:
    glm::vec3 color = glm::vec3(1.0f); // ��Դ��ɫ
    float intensity = 1.0f;            // ��Դǿ��
};