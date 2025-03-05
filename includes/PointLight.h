// PointLight.h
#pragma once
#include "Light.h"

struct attenuation {
    float constant; // 衰减常数项
    float linear;   // 衰减一次项
    float quadratic;    // 衰减二次项
};

class PointLight : public Light {
public:
    PointLight(const glm::vec3& position, attenuation att) : position(position), att(att) {}
    ~PointLight() {}

    // 设置光源位置
    void SetPosition(const glm::vec3& position) { this->position = position; }
    glm::vec3 GetPosition() const { return position; }

    // 设置衰减参数
    void SetAttenuation(float constant, float linear, float quadratic) {
        this->att.constant = constant;
        this->att.linear = linear;
        this->att.quadratic = quadratic;
    }

    // 获取衰减参数
    void GetAttenuation(float& constant, float& linear, float& quadratic) const {
        constant = this->att.constant;
        linear = this->att.linear;
        quadratic = this->att.quadratic;
    }

    LightType GetType() const override {
        return Point;
    }
public:
    glm::vec3 position; // 光源位置
    attenuation att;
};