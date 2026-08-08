#pragma once

#include <glm/vec3.hpp>

struct DebugSettings
{
    // Render
    bool bWireframe = false;

    // Lighting
    bool bAnimateLight = false;
    float LightIntensity = 1.0f;
    glm::vec3 LightPosition = {2.0f, 3.0f, 0.0f};

    // Background
    float BackgroundColor[3] = {0.2f, 0.3f, 0.3f};
};