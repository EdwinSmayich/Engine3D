#include "AppContext.h"
#include <iostream>
#include "../../Textures/stb_image.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

void AppContext::ResetAppContextToDefaults()
{
    Settings = DebugSettings{};

    // Light properties
    Lights = AppContext{}.Lights;
    SelectedLight = 0;
}

namespace FTexture
{
    GLuint LoadTexture(const char* InPath)
    {
        GLuint TextureID = 0;
        glGenTextures(1, &TextureID);

        GLint Width = 0, Height = 0, nrChannels = 0;
        GLubyte* TextureData = stbi_load(InPath, &Width, &Height, &nrChannels, 0);

        if (TextureData)
        {
            GLenum Format = 0;
            if (nrChannels == 1)
            {
                Format = GL_RED;
            }
            else if (nrChannels == 3)
            {
                Format = GL_RGB;
            }
            else if (nrChannels == 4)
            {
                Format = GL_RGBA;
            }

            // Bind texture
            glBindTexture(GL_TEXTURE_2D, TextureID);

            // Upload texture to GPU
            glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, TextureData);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Texture wrapping mode
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Texture filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(TextureData);
        }
        else
        {
            std::cerr << "Texture failed to load at path: " << InPath << "\n";
            stbi_image_free(TextureData);
        }

        return TextureID;
    }
} // namespace FTexture

namespace FCallBack
{
    const GLvoid* BufferOffset(size_t InBytes)
    {
        return reinterpret_cast<GLvoid*>(InBytes);
    }

    void FrameBufferSizeCallback(GLFWwindow*, int InWidth, int InHeight)
    {
        glViewport(0, 0, InWidth, InHeight);
    }

    void MouseCursorPosCallback(GLFWwindow* InWindow, GLdouble InPosX, GLdouble InPosY)
    {
        auto* Ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(InWindow));

        if (Ctx->bCursorModeActive)
        {
            if (Ctx->bDraggingLight)
            {
                int W, H;
                glfwGetWindowSize(InWindow, &W, &H); // that exact current size
                glm::vec3 Ray = FUI::ScreenToWorldRay(InPosX, InPosY, W, H, Ctx->Projection, Ctx->View);

                glm::vec3 O = Ctx->MainCamera.GetPosition();
                glm::vec3 N = Ctx->MainCamera.GetFrontVector();         // the plane is facing the camera
                glm::vec3 P = Ctx->Lights[Ctx->SelectedLight].Position; // passes through the lamp

                glm::vec3 Hit;
                if (FPhysics::RayHitsPlane(O, Ray, P, N, Hit))
                {
                    Ctx->Lights[Ctx->SelectedLight].Position = Hit; // The lamp follows the cursor
                }
            }

            Ctx->bFirstMouse = true;
            return;
        }

        if (Ctx->bFirstMouse)
        {
            Ctx->LastX = static_cast<float>(InPosX);
            Ctx->LastY = static_cast<float>(InPosY);
            Ctx->bFirstMouse = false;
            return;
        }

        GLfloat OffsetX = static_cast<float>(InPosX) - Ctx->LastX;
        GLfloat OffsetY = Ctx->LastY - static_cast<float>(InPosY); // Reversed since y-coordinates range from bottom to top

        Ctx->LastX = static_cast<float>(InPosX);
        Ctx->LastY = static_cast<float>(InPosY);

        Ctx->MainCamera.ProcessMouseMovement(OffsetX, OffsetY, GL_TRUE);
    }

    void MouseButtonCallback(GLFWwindow* InWindow, GLint InButton, GLint InAction, GLint /*Mods*/)
    {
        auto* Ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(InWindow));

        // clang-forma off
        if (!Ctx->bCursorModeActive)
            return;
        if (ImGuizmo::IsOver())
            return;
        if (InButton != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (InAction == GLFW_RELEASE)
        {
            Ctx->bDraggingLight = false;
            return;
        }

        if (InAction != GLFW_PRESS)
            return;
        if (ImGui::GetIO().WantCaptureMouse)
            return;
        // clang-forma on

        double MouseX, MouseY;
        glfwGetCursorPos(InWindow, &MouseX, &MouseY);

        int Width, Height;
        glfwGetWindowSize(InWindow, &Width, &Height);

        glm::vec3 Ray = FUI::ScreenToWorldRay(MouseX, MouseY, Width, Height, Ctx->Projection, Ctx->View);
        glm::vec3 O = Ctx->MainCamera.GetPosition();
        int Hit = -1;
        float Nearest = std::numeric_limits<float>::max();

        for (int i = 0; i < static_cast<int>(Ctx->Lights.size()); ++i)
        {
            float T;
            if (FPhysics::RayHitsSphere(O, Ray, Ctx->Lights[i].Position, 0.4f, T) && T < Nearest)
            {
                Nearest = T; // This hit is closer than the previous ones
                Hit = i;
            }
        }

        if (Hit != -1)
        {
            Ctx->SelectedLight = Hit; // chose the lamp that the beam hit
            Ctx->bDraggingLight = true;
        }
    }

    void ScrollCallback(GLFWwindow* InWindow, GLdouble, GLdouble InOffsetY)
    {
        if (ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }

        auto* Ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(InWindow));
        Ctx->MainCamera.ProcessMouseScroll(static_cast<GLfloat>(InOffsetY));
    }

    // Process all input: query GLFW whether relevant keys are pressed/released this frame and react
    // accordingly
    void ProcessInput(GLFWwindow* InWindow, GLfloat InDeltaTime, AppContext& InContext)
    {
        if (glfwGetKey(InWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(InWindow, GLFW_TRUE);
        }

        // Place the cursor over the ImGui interface menu
        static bool bTapPressedLastFrame = false;
        bool bTapPressedNow = (glfwGetKey(InWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);

        if (bTapPressedNow && !bTapPressedLastFrame)
        {
            InContext.bCursorModeActive = !InContext.bCursorModeActive;

            if (InContext.bCursorModeActive)
            {
                glfwSetInputMode(InWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetCursorPos(InWindow, CENTER_SCREEN.x, CENTER_SCREEN.y);
                InContext.bFirstMouse = true;
            }
            else
            {
                glfwSetInputMode(InWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                InContext.bFirstMouse = true;
            }
        }

        bTapPressedLastFrame = bTapPressedNow;

        if (InContext.bCursorModeActive)
        {
            return;
        }

        // Camera movement
        if (glfwGetKey(InWindow, GLFW_KEY_W) == GLFW_PRESS)
        {
            InContext.MainCamera.ProcessKeyboard(CameraMovementType::CMT_Forward, InDeltaTime);
        }
        if (glfwGetKey(InWindow, GLFW_KEY_S) == GLFW_PRESS)
        {
            InContext.MainCamera.ProcessKeyboard(CameraMovementType::CMT_Backward, InDeltaTime);
        }
        if (glfwGetKey(InWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            InContext.MainCamera.ProcessKeyboard(CameraMovementType::CMT_Up, InDeltaTime);
        }
        if (glfwGetKey(InWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            InContext.MainCamera.ProcessKeyboard(CameraMovementType::CMT_Down, InDeltaTime);
        }
        if (glfwGetKey(InWindow, GLFW_KEY_D) == GLFW_PRESS)
        {
            InContext.MainCamera.ProcessKeyboard(CameraMovementType::CMT_Right, InDeltaTime);
        }
        if (glfwGetKey(InWindow, GLFW_KEY_A) == GLFW_PRESS)
        {
            InContext.MainCamera.ProcessKeyboard(CameraMovementType::CMT_Left, InDeltaTime);
        }
    }

} // namespace FCallBack

namespace FUI
{
    glm::vec3 ScreenToWorldRay(GLdouble InMouseX, GLdouble InMouseY, GLint InWidth, GLint InHeight, const glm::mat4& InProj, const glm::mat4& InView)
    {
        GLfloat X = (2.0f * static_cast<GLfloat>(InMouseX)) / static_cast<GLfloat>(InWidth) - 1.0f;
        GLfloat Y = 1.0f - (2.0f * static_cast<GLfloat>(InMouseY)) / static_cast<GLfloat>(InHeight);

        glm::vec4 RayClip = glm::vec4(X, Y, -1.0f, 1.0f);
        glm::vec4 RayView = glm::inverse(InProj) * RayClip;

        RayView = glm::vec4(RayView.x, RayView.y, -1.0f, 0.0f);
        glm::vec3 RayWorld = glm::normalize(glm::vec3(glm::inverse(InView) * RayView));

        return RayWorld;
    }
} // namespace FUI

namespace FPhysics
{
    // clang-format off
    bool RayHitsSphere(glm::vec3 InO, glm::vec3 InD, glm::vec3 InC, GLfloat InR, GLfloat& OutT)
    {
        glm::vec3 OC = InC - InO;                       // from the camera to the center of the sphere
        float Tca = glm::dot(OC, InD);              // projection of OC onto the ray (where the closest approach occurs)
        if (Tca < 0.0f)
        {
            return false;                               // sphere BEHIND the ray — misses
        }

        float D2 = glm::dot(OC, OC) - Tca * Tca;    // square of the distance from the center to the ray line
        float R2 = InR * InR;
        if (D2 > R2)
        {
            return false;                               // the ray passes BY the sphere
        }

        float Thc = glm::sqrt(R2 - D2);              // half of the chord inside the sphere
        OutT = Tca - Thc;                               // distance to the NEAREST entry point
        return true;
    }
    
    bool RayHitsPlane(glm::vec3 InO, glm::vec3 InD, glm::vec3 InPlanePoint, glm::vec3 InPlaneNormal, glm::vec3& OutHit)
    {
        float Denom = glm::dot(InD, InPlaneNormal);
        if (glm::abs(Denom) < 1e-6f)
        {
            return false;                               // If a ray is parallel to a plane, it will not intersect it.
        }
        
        float T = glm::dot(InPlanePoint - InO, InPlaneNormal) / Denom;
        if (T < 0.0f)
        {
            return false;                               // the plane behind the camera
        }
        
        OutHit = InO + T * InD;                         // point of impact
        return true;
    }
    // clang-format on
} // namespace FPhysics