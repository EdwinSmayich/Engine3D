#pragma once

#include "DebugSettings.h"
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
};