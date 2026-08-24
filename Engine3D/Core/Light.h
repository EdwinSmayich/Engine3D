#pragma once
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>

enum class LightType : uint8_t
{
    ELT_Directional = 0,
    ELT_Point = 1,
    ELT_Spot = 2
};

struct FLight
{
    glm::vec3 Direction = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 Diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // Attenuation
    float Constant = 1.0f;
    float Linear = 0.045f;
    float Quadratic = 0.0075f;

    float InnerCutoff = glm::cos(glm::radians(12.0f)); // in degrees
    float OuterCutoff = glm::cos(glm::radians(18.0f)); // in degrees

    bool bAnimateLight = false;

    LightType LightingType = LightType::ELT_Point;
};
