#pragma once

#include "DebugSettings.h"
#include "Light.h"
#include "Materials.h"
#include "../Camera/Camera.h"

struct AppContext
{
    // ImGui Entities
    DebugSettings Settings;

    // Camera
    Camera MainCamera;

    // Screen settings
    float LastX = 0.0f;
    float LastY = 0.0f;
    bool bFirstMouse = true;
    bool bCursorModeActive = false; // false means that the mouse is hidden (we're controlling the camera)

    // Material
    MaterialLibrary Materials;

    // Scene
    std::vector<Light> Lights{
        {glm::vec3(-4.0f, 3.0f, -3.5f), glm::vec3(1.0f, 1.0f, 1.0f)}, // White
        {glm::vec3(6.0f, 2.0f, -2.0f), glm::vec3(1.0f, 0.3f, 0.3f)},  // Reddish
        {glm::vec3(0.0f, 5.0f, -8.0f), glm::vec3(0.3f, 0.4f, 1.0f)},  // Bluish
    };

    int SelectedLight = 0; // Which light we are editing right now
};

namespace FTexture
{
    GLuint LoadTexture(const char* InPath);
} // namespace FTexture
