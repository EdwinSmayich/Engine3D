#pragma once

struct FDebugSettings
{
    // Render
    bool bWireframe = false;

    // Background
    float BackgroundColor[3] = {0.1f, 0.1f, 0.1f};

    // Lighting
    float AmbientStrength = 0.15f;
};