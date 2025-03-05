// PointLight.h
#pragma once
#include "Light.h"

struct attenuation {
    float constant; // ˥��������
    float linear;   // ˥��һ����
    float quadratic;    // ˥��������
};

class PointLight : public Light {
public:
    PointLight(const glm::vec3& position, attenuation att) : position(position), att(att) {}
    ~PointLight() {}

    // ���ù�Դλ��
    void SetPosition(const glm::vec3& position) { this->position = position; }
    glm::vec3 GetPosition() const { return position; }

    // ����˥������
    void SetAttenuation(float constant, float linear, float quadratic) {
        this->att.constant = constant;
        this->att.linear = linear;
        this->att.quadratic = quadratic;
    }

    // ��ȡ˥������
    void GetAttenuation(float& constant, float& linear, float& quadratic) const {
        constant = this->att.constant;
        linear = this->att.linear;
        quadratic = this->att.quadratic;
    }

    LightType GetType() const override {
        return Point;
    }
public:
    glm::vec3 position; // ��Դλ��
    attenuation att;
};