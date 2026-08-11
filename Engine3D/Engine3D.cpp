// GLAD must be included BEFORE GLFW - it provides the OpenGL headers.
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "ImGui/ImGuiLayer.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include "Shader.h"
#include "Camera/Camera.h"
#include "Core/AppContext.h"
#include "Core/DebugSettings.h"

static void FrameBufferSizeCallback(GLFWwindow*, int InWidth, int InHeight);
static void MouseCallBack(GLFWwindow* InWindow, GLdouble InPosX, GLdouble InPosY);
static void ScrollCallBack(GLFWwindow* InWindow, GLdouble, GLdouble InOffsetY);
static void ProcessInput(GLFWwindow* InWindow, GLfloat InDeltaTime, AppContext& InContext);

inline const GLvoid* BufferOffset(size_t InBytes);

// Screen settings
constexpr GLint WIDTH_SCREEN = 1920;
constexpr GLint HEIGHT_SCREEN = 1200;
constexpr glm::vec2 CENTER_SCREEN = glm::vec2(WIDTH_SCREEN * 0.5f, HEIGHT_SCREEN * 0.5f);

int main()
{
    // --- GLFW, window and OpenGL context -------------------------------------
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* Window = glfwCreateWindow(WIDTH_SCREEN, HEIGHT_SCREEN, "Engine3D", nullptr, nullptr);
    if (!Window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(Window);
    AppContext Context;
    glfwSetWindowUserPointer(Window, &Context);

    GLint Version = gladLoadGL(glfwGetProcAddress);
    if (!Version)
    {
        std::cerr << "Failed to initialize OpenGL context (GLAD)\n";

        glfwDestroyWindow(Window);
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(Window, FrameBufferSizeCallback);
    glfwSetCursorPosCallback(Window, MouseCallBack);
    glfwSetScrollCallback(Window, ScrollCallBack);

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, GL_TRUE);
    ImGui_ImplOpenGL3_Init("#version 330");

    GLint FbWidth = 0, FbHeight = 0;
    glfwGetFramebufferSize(Window, &FbWidth, &FbHeight);
    FrameBufferSizeCallback(Window, FbWidth, FbHeight);

    glEnable(GL_DEPTH_TEST);

    // Create shader program
    Shader CubeShader(SHADER_DIR "/3.3.Shader.vert", SHADER_DIR "/3.3.Shader.frag");
    Shader LightingCubeShader(SHADER_DIR "/LightCube.vert", SHADER_DIR "/LightCube.frag");

    // clang-format off
    GLfloat Vertices[] = {
        // Red facet
        // Position           // Color            // Normal
        -1.0f, -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
  
        // Green facet 
        // Position           // Color            // Normal
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, -1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
         
        // Blue facet 
        // Position           // Color            // Normal
        1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
        
        // Yellow facet  
        // Position           // Color            // Normal
        -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
        
        // Magenta facet  
        // Position           // Color            // Normal
        -1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
        
        // White facet  
        // Position           // Color            // Normal
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
    };
    
    GLuint Indices[] = {
        // Front
        0,1,2,
        0,2,3,

        // Back
        4,5,6,
        4,6,7,

        // Right
        8,9,10,
        8,10,11,
        
        // Left
        12,13,14,
        12,14,15,
        
        // Top
        16,17,18,
        16,18,19,
        
        // Bottom
        20,21,22,
        20,22,23
    };
    // clang-format on

    // Vertex Array
    GLuint CubesVAO = 0;
    glGenVertexArrays(1, &CubesVAO);
    glBindVertexArray(CubesVAO);

    // Vertex Buffer
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    GLuint CubesEBO = 0;
    glGenBuffers(1, &CubesEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BufferOffset(0));
    glEnableVertexAttribArray(0);

    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BufferOffset(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Lighting scene
    GLuint LightVAO = 0;
    glGenVertexArrays(1, &LightVAO);
    glBindVertexArray(LightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubesEBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BufferOffset(0));
    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLfloat LastFrame = 0.0f;

    while (!glfwWindowShouldClose(Window))
    {
        auto* Ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(Window));

        // Per-frame time logic
        GLfloat CurrentFrame = static_cast<GLfloat>(glfwGetTime());
        GLfloat DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        // ImGui the interface
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiLayer::BuildUI(Ctx->Settings, Ctx->MainCamera);

        if (Ctx->Settings.bWireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Input
        ProcessInput(Window, DeltaTime, Context);

        glClearColor(Ctx->Settings.BackgroundColor[0], Ctx->Settings.BackgroundColor[1], Ctx->Settings.BackgroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CubeShader.Use();
        // Light properties
        CubeShader.SetVec3("uLight.Position", Ctx->Settings.LightPosition);
        CubeShader.SetVec3("uViewPos", Ctx->MainCamera.GetPosition());

        CubeShader.SetVec3("uLight.LightColor", glm::vec3(Ctx->Settings.LightColor[0], Ctx->Settings.LightColor[1], Ctx->Settings.LightColor[2]));
        CubeShader.SetVec3("uLight.Ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        CubeShader.SetVec3("uLight.Diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        CubeShader.SetVec3("uLight.Specular", glm::vec3(1.0f, 1.0f, 1.0f));
        CubeShader.SetFloat("uLight.AmbientStrength", Ctx->Settings.LightAmbient);
        CubeShader.SetFloat("uLight.SpecularStrength", Ctx->Settings.LightSpecular);

        // Projection transformation
        constexpr GLfloat Aspect = static_cast<GLfloat>(WIDTH_SCREEN) / static_cast<GLfloat>(HEIGHT_SCREEN);
        const GLfloat FOV = Ctx->MainCamera.GetFOV();
        glm::mat4 Projection;
        Projection = glm::perspective(glm::radians(FOV), Aspect, 0.1f, 200.0f);
        CubeShader.SetMat4("uProjection", Projection);

        // View transformation
        glm::mat4 View = Ctx->MainCamera.GetViewMatrix();
        CubeShader.SetMat4("uView", View);

        // Render cubes
        glBindVertexArray(CubesVAO);
        for (int i = 0; i < Ctx->Materials.GetMaterialsNames().size(); ++i)
        {
            GLfloat OffsetX = 3.0f;

            // Material properties
            const Material& Material = Ctx->Materials.Get(Ctx->Materials.GetMaterialsNames()[i]);
            CubeShader.SetVec3("uMaterial.Ambient", Material.Ambient);
            CubeShader.SetVec3("uMaterial.Diffuse", Material.Diffuse);
            CubeShader.SetVec3("uMaterial.Specular", Material.Specular);
            CubeShader.SetFloat("uMaterial.Shininess", Material.Shininess);

            // World/Model transformation
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(i * OffsetX, 0.0f, -3.0f));
            Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 5.0f));
            CubeShader.SetMat4("uModel", Model);

            // Normal Matrix
            glm::mat3 NormalMatrix = glm::mat3(glm::transpose(glm::inverse(Model)));
            CubeShader.SetMat3("uNormalMatrix", NormalMatrix);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }

        // Lighting cube
        LightingCubeShader.Use();
        LightingCubeShader.SetVec3("uLightColor", glm::vec3(Ctx->Settings.LightColor[0], Ctx->Settings.LightColor[1], Ctx->Settings.LightColor[2]));
        if (Ctx->Settings.bAnimateLight)
        {
            Ctx->Settings.LightPosition.y = glm::sin(CurrentFrame) * 7.0f;
            Ctx->Settings.LightPosition.z = -(glm::cos(CurrentFrame) * 0.5f + 0.5f) * 7.0f;
        }
        glm::mat4 LightCubeModel(1.0f);
        LightCubeModel = glm::translate(LightCubeModel, Ctx->Settings.LightPosition);
        LightCubeModel = glm::scale(LightCubeModel, glm::vec3(0.2f));
        LightingCubeShader.SetMat4("uProjection", Projection);
        LightingCubeShader.SetMat4("uView", View);
        LightingCubeShader.SetMat4("uModel", LightCubeModel);

        // Draw the light cube
        glBindVertexArray(LightVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        // ImGui render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &CubesVAO);
    glDeleteBuffers(1, &CubesEBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

static void FrameBufferSizeCallback(GLFWwindow*, int InWidth, int InHeight)
{
    glViewport(0, 0, InWidth, InHeight);
}

static void MouseCallBack(GLFWwindow* InWindow, GLdouble InPosX, GLdouble InPosY)
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

static void ScrollCallBack(GLFWwindow* InWindow, GLdouble, GLdouble InOffsetY)
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
static void ProcessInput(GLFWwindow* InWindow, GLfloat InDeltaTime, AppContext& InContext)
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

    // if (glfwGetKey(InWindow, GLFW_KEY_Q && GLFW_RAW_MOUSE_MOTION) == GLFW_PRESS)
    // {
    //     GLfloat DeltaSpeed = InContext.MainCamera.GetSpeed() * glfwRawMouseMotionSupported();
    //     InContext.MainCamera.SetSpeed(DeltaSpeed);
    // }
}

inline const GLvoid* BufferOffset(size_t InBytes)
{
    return reinterpret_cast<GLvoid*>(InBytes);
}
