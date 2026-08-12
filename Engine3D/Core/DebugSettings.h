#pragma once

#include <glm/vec3.hpp>

struct DebugSettings
{
    // Render
    bool bWireframe = false;

    // Background
    float BackgroundColor[3] = {0.1f, 0.1f, 0.1f};

    // Lighting
    // bool bAnimateLight = false;
    float AmbientStrength = 0.15f;
    float DiffuseStrength = 1.0f;
    float SpecularStrength = 1.0f;
    // glm::vec3 LightPosition = glm::vec3(-4.0f, 3.0f, -3.5f);
    // float LightColor[3] = {1.0f, 1.0f, 1.0f};
};