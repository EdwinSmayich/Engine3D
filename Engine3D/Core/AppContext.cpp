#include "AppContext.h"
#include <iostream>
#include "../../Textures/stb_image.h"

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

    void MouseCallBack(GLFWwindow* InWindow, GLdouble InPosX, GLdouble InPosY)
    {
        auto* Ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(InWindow));

        if (Ctx->bCursorModeActive)
        {
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

    void ScrollCallBack(GLFWwindow* InWindow, GLdouble, GLdouble InOffsetY)
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