// GLAD must be included BEFORE GLFW - it provides the OpenGL headers.
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <imgui.h>
#include "ImGui/ImGuiLayer.h"
#include "ImGuizmo.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include "Shader.h"
#include "Core/AppContext.h"
#include "glm/gtc/type_ptr.inl"

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

    glfwSetFramebufferSizeCallback(Window, FCallBack::FrameBufferSizeCallback);
    glfwSetCursorPosCallback(Window, FCallBack::MouseCursorPosCallback);
    glfwSetMouseButtonCallback(Window, FCallBack::MouseButtonCallback);
    glfwSetScrollCallback(Window, FCallBack::ScrollCallback);

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window, GL_TRUE);
    ImGui_ImplOpenGL3_Init("#version 330");

    GLint FbWidth = 0, FbHeight = 0;
    glfwGetFramebufferSize(Window, &FbWidth, &FbHeight);
    FCallBack::FrameBufferSizeCallback(Window, FbWidth, FbHeight);

    glEnable(GL_DEPTH_TEST);

    // Create shader program
    Shader CubeShader(SHADER_DIR "/3.3.Shader.vert", SHADER_DIR "/3.3.Shader.frag");
    Shader LightingCubeShader(SHADER_DIR "/LightCube.vert", SHADER_DIR "/LightCube.frag");

    // clang-format off
    GLfloat Vertices[] = {
        // Red facet
        // Position           // Color            // Normal            // UV
        -1.0f, -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
  
        // Green facet 
        // Position           // Color            // Normal            // UV
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
         
        // Blue facet 
        // Position           // Color            // Normal            // UV
        1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        
        // Yellow facet  
        // Position           // Color            // Normal            // UV
        -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
        
        // Magenta facet  
        // Position           // Color            // Normal            // UV
        -1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        
        // White facet  
        // Position           // Color            // Normal            // UV
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
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
    
    glm::vec3 CubePositions[] = {
        glm::vec3( -5.0f,  0.0f,  -3.0f), 
        glm::vec3( 5.0f, -2.4f, -5.5f),  
        glm::vec3(-4.5f, -4.2f, -2.5f),  
        glm::vec3( 5.0f,  7.0f, -15.0f), 
        glm::vec3(-7.8f, -4.0f, -12.3f),  
        glm::vec3(-4.7f,  5.0f, -7.5f),  
        glm::vec3( 2.3f, -4.0f, -2.5f),  
        glm::vec3( 4.5f,  4.0f, -2.5f), 
        glm::vec3( 4.5f,  1.2f, -1.5f), 
        glm::vec3(-4.3f,  2.0f, -1.5f)  
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), FCallBack::BufferOffset(0));
    glEnableVertexAttribArray(0);

    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), FCallBack::BufferOffset(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), FCallBack::BufferOffset(9 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Create textures
    GLuint DiffuseMap = FTexture::LoadTexture(TEXTURE_DIR "/Container2.png");
    GLuint SpecularMap = FTexture::LoadTexture(TEXTURE_DIR "/Container2_Specular.png");
    GLuint EmissionMap = FTexture::LoadTexture(TEXTURE_DIR "/Matrix.jpg");

    // Lighting scene
    GLuint LightVAO = 0;
    glGenVertexArrays(1, &LightVAO);
    glBindVertexArray(LightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubesEBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), FCallBack::BufferOffset(0));
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
        ImGuizmo::BeginFrame();
        FImGuiLayer::BuildUI(*Ctx);

        if (Ctx->Settings.bWireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Input
        FCallBack::ProcessInput(Window, DeltaTime, Context);

        glClearColor(Ctx->Settings.BackgroundColor[0], Ctx->Settings.BackgroundColor[1], Ctx->Settings.BackgroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Projection transformation
        constexpr GLfloat Aspect = static_cast<GLfloat>(WIDTH_SCREEN) / static_cast<GLfloat>(HEIGHT_SCREEN);
        const GLfloat FOV = Ctx->MainCamera.GetFOV();
        glm::mat4 Projection;
        Projection = glm::perspective(glm::radians(FOV), Aspect, 0.1f, 200.0f);

        // View transformation
        glm::mat4 View = Ctx->MainCamera.GetViewMatrix();

        Ctx->Projection = Projection;
        Ctx->View = View;

        // Lighting cube
        // ===================================================================================================
        LightingCubeShader.Use();
        LightingCubeShader.SetMat4("uProjection", Projection);
        LightingCubeShader.SetMat4("uView", View);

        // Render light cubes
        glBindVertexArray(LightVAO);
        for (size_t i = 0; i < Ctx->SceneObjects.size(); ++i)
        {
            FSceneObject& Light = Ctx->SceneObjects[i]; // TODO: mb set conts here
            bool bSelected = (i == Ctx->SelectedObject);

            GLfloat LampScale = bSelected ? 0.32f : 0.2f; // The selected one is noticeably larger
            LightingCubeShader.SetVec3("uLightColor", bSelected ? glm::vec3(1.0f) : Light.LightData.Color);

            if (Light.LightData.bAnimateLight)
            {
                GLfloat Speed = 3.0f;
                Light.Transform.Position.y = glm::sin(CurrentFrame * Speed) * 7.0f;
                Light.Transform.Position.z = -(glm::cos(CurrentFrame * Speed) * 0.5f + 0.5f) * 7.0f;
            }

            glm::mat4 LightModelMatrix(1.0f);
            LightModelMatrix = glm::translate(LightModelMatrix, Light.Transform.Position);
            LightModelMatrix = glm::scale(LightModelMatrix, glm::vec3(LampScale));
            LightingCubeShader.SetMat4("uModel", LightModelMatrix);

            // Draw light cubes
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }

        // Cubes/Containers properties
        // ===================================================================================================
        CubeShader.Use();
        CubeShader.SetMat4("uProjection", Projection);
        CubeShader.SetMat4("uView", View);

        // Material properties
        const Material& Material = Ctx->Materials.Get("Emerald");
        CubeShader.SetInt("uMaterial.Diffuse", 0);
        CubeShader.SetInt("uMaterial.Specular", 1);
        CubeShader.SetInt("uMaterial.Emission", 2);
        CubeShader.SetFloat("uMaterial.Shininess", Material.Shininess);

        // Lighting
        CubeShader.SetInt("uLightCount", Ctx->SceneObjects.size());
        CubeShader.SetVec3("uViewPos", Ctx->MainCamera.GetPosition());

        for (size_t i = 0; i < Ctx->SceneObjects.size(); ++i)
        {
            FSceneObject& Light = Ctx->SceneObjects[i];

            std::string Base = "uLights[" + std::to_string(i) + "]";
            CubeShader.SetVec3(Base + ".Position", glm::vec3(Light.Transform.Position));
            CubeShader.SetVec3(Base + ".Direction", glm::vec3(Light.LightData.Direction));
            CubeShader.SetVec3(Base + ".Color", glm::vec3(Light.LightData.Color));
            CubeShader.SetVec3(Base + ".Ambient", glm::vec3(Light.LightData.Ambient));
            CubeShader.SetVec3(Base + ".Diffuse", glm::vec3(Light.LightData.Diffuse));
            CubeShader.SetVec3(Base + ".Specular", glm::vec3(Light.LightData.Specular));
            CubeShader.SetFloat(Base + ".Constant", Light.LightData.Constant);
            CubeShader.SetFloat(Base + ".Linear", Light.LightData.Linear);
            CubeShader.SetFloat(Base + ".Quadratic", Light.LightData.Quadratic);
            CubeShader.SetFloat(Base + ".InnerCutoff", Light.LightData.InnerCutoff);
            CubeShader.SetFloat(Base + ".OuterCutoff", Light.LightData.OuterCutoff);
            CubeShader.SetInt(Base + ".LightingType", static_cast<int>(Light.LightData.LightingType));
        }

        // Bind Diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DiffuseMap);

        // Bind Specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SpecularMap);

        // Bind emission map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, EmissionMap);

        // Render cubes
        glBindVertexArray(CubesVAO);
        for (size_t i = 0; i < NUM_CUBES; ++i)
        {
            // World/Model transformation
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, CubePositions[i]);
            GLfloat Angle = CurrentFrame * 100.0f;
            Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(1.0f, 0.3f, 0.1f));
            CubeShader.SetMat4("uModel", Model);

            // Normal Matrix
            glm::mat3 NormalMatrix = glm::mat3(glm::transpose(glm::inverse(Model)));
            CubeShader.SetMat3("uNormalMatrix", NormalMatrix);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }

        // ImGuizmo render
        if (!Ctx->SceneObjects.empty())
        {
            ImGuiIO& IO = ImGui::GetIO();
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetRect(0.0f, 0.0f, IO.DisplaySize.x, IO.DisplaySize.y);

            glm::mat4 GizmoModel = glm::translate(glm::mat4(1.0f), Ctx->SceneObjects[Ctx->SelectedObject].Transform.Position);
            ImGuizmo::Manipulate(glm::value_ptr(View), glm::value_ptr(Projection), ImGuizmo::TRANSLATE, ImGuizmo::WORLD, glm::value_ptr(GizmoModel));

            if (ImGuizmo::IsUsing())
            {
                Ctx->SceneObjects[Ctx->SelectedObject].Transform.Position = glm::vec3(GizmoModel[3]);
            }
        }

        // ImGui render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &CubesVAO);
    glDeleteVertexArrays(1, &LightVAO);
    glDeleteBuffers(1, &CubesEBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
