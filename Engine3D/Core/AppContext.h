#pragma once

#include "DebugSettings.h"
#include "Light.h"
#include "Materials.h"
#include "../Camera/Camera.h"
#include "GLFW/glfw3.h"

// Screen settings
constexpr GLint WIDTH_SCREEN = 1920;
constexpr GLint HEIGHT_SCREEN = 1200;
constexpr glm::vec2 CENTER_SCREEN = glm::vec2(WIDTH_SCREEN * 0.5f, HEIGHT_SCREEN * 0.5f);

struct AppContext
{
    glm::mat4 Projection;
    glm::mat4 View;

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
    std::vector<FLight> Lights{
        {glm::vec3(-4.0f, 3.0f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f)}, // White
        // {glm::vec3(6.0f, 2.0f, -2.0f), glm::vec3(1.0f, 0.3f, 0.3f)},  // Reddish
        // {glm::vec3(0.0f, 5.0f, -8.0f), glm::vec3(0.3f, 0.4f, 1.0f)},  // Bluish
    };

    int SelectedLight = 0;       // Which light we are editing right now
    bool bDraggingLight = false; // Status: "I'm draging it right now"

    void ResetAppContextToDefaults();
};

namespace FTexture
{
    GLuint LoadTexture(const char* InPath);
} // namespace FTexture

namespace FCallBack
{
    const GLvoid* BufferOffset(size_t InBytes);
    void FrameBufferSizeCallback(GLFWwindow*, GLint InWidth, GLint InHeight);
    void MouseCursorPosCallback(GLFWwindow* InWindow, GLdouble InPosX, GLdouble InPosY);
    void MouseButtonCallback(GLFWwindow* InWindow, GLint InButton, GLint InAction, GLint /*Mods*/);
    void ScrollCallback(GLFWwindow* InWindow, GLdouble, GLdouble InOffsetY);
    void ProcessInput(GLFWwindow* InWindow, GLfloat InDeltaTime, AppContext& InContext);
} // namespace FCallBack

namespace FUI
{
    glm::vec3 ScreenToWorldRay(GLdouble InMouseX, GLdouble InMouseY, GLint InWidth, GLint InHeight, const glm::mat4& InProj, const glm::mat4& InView);
} // namespace FUI