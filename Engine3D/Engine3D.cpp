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
#include <assimp/Importer.hpp>
#include <assimp/version.h>

#include <iostream>
#include "Shader.h"
#include "Core/AppContext.h"
#include "glm/gtc/type_ptr.inl"
#include "glm/gtx/matrix_decompose.hpp"

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
    FAppContext Context;
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
        auto* Ctx = static_cast<FAppContext*>(glfwGetWindowUserPointer(Window));

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
            if (Ctx->SceneObjects[i].ObjectType != EObjectType::EOT_Light)
            {
                continue;
            }

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

            glm::mat4 LightModelMatrix = Light.Transform.GetMatrix();
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
        CubeShader.SetVec3("uViewPos", Ctx->MainCamera.GetTransform().Position);

        GLint LightIndex = 0;
        for (size_t i = 0; i < Ctx->SceneObjects.size(); ++i)
        {
            if (Ctx->SceneObjects[i].ObjectType != EObjectType::EOT_Light)
            {
                continue;
            }

            FSceneObject& Light = Ctx->SceneObjects[i];
            std::string Base = "uLights[" + std::to_string(LightIndex) + "]";
            CubeShader.SetVec3(Base + ".Position", glm::vec3(Light.Transform.Position));
            CubeShader.SetVec3(Base + ".Direction", glm::vec3(Light.LightData.Direction));
            CubeShader.SetVec3(Base + ".Color", glm::vec3(Light.LightData.Color));
            CubeShader.SetVec3(Base + ".Diffuse", glm::vec3(Light.LightData.Diffuse));
            CubeShader.SetVec3(Base + ".Specular", glm::vec3(Light.LightData.Specular));
            CubeShader.SetFloat(Base + ".Constant", Light.LightData.Constant);
            CubeShader.SetFloat(Base + ".Linear", Light.LightData.Linear);
            CubeShader.SetFloat(Base + ".Quadratic", Light.LightData.Quadratic);
            CubeShader.SetFloat(Base + ".InnerCutoff", Light.LightData.InnerCutoff);
            CubeShader.SetFloat(Base + ".OuterCutoff", Light.LightData.OuterCutoff);
            CubeShader.SetInt(Base + ".LightingType", static_cast<int>(Light.LightData.LightingType));

            ++LightIndex;
        }
        CubeShader.SetInt("uLightCount", LightIndex);
        CubeShader.SetFloat("uAmbientStrength", Ctx->Settings.AmbientStrength);

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
        for (FSceneObject& Obj : Ctx->SceneObjects)
        {
            if (Obj.ObjectType != EObjectType::EOT_Cube)
            {
                continue;
            }
            // World/Model transformation
            GLfloat AngularSpeed = glm::radians(45.0f) * DeltaTime;
            glm::quat DeltaRotation = glm::angleAxis(AngularSpeed, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
            Obj.Transform.Rotation = glm::normalize(DeltaRotation * Obj.Transform.Rotation);

            glm::mat4 Model = Obj.Transform.GetMatrix();
            CubeShader.SetMat4("uModel", Model);

            // Normal Matrix
            glm::mat3 NormalMatrix = glm::mat3(glm::transpose(glm::inverse(Model)));
            CubeShader.SetMat3("uNormalMatrix", NormalMatrix);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }

        // ImGuizmo render
        if (!Ctx->SceneObjects.empty())
        {
            FTransform& TransformObj = Ctx->SceneObjects[Ctx->SelectedObject].Transform;

            ImGuiIO& IO = ImGui::GetIO();
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetRect(0.0f, 0.0f, IO.DisplaySize.x, IO.DisplaySize.y);

            ImGuizmo::OPERATION Operation = ImGuizmo::TRANSLATE;
            if (Ctx->GizmoOperation == EGizmoOperation::EGO_Rotate)
            {
                Operation = ImGuizmo::ROTATE;
            }
            else if (Ctx->GizmoOperation == EGizmoOperation::EGO_Scale)
            {
                Operation = ImGuizmo::SCALE;
            }

            ImGuizmo::MODE Mode = (Ctx->GizmoMode == EGizmoMode::EGM_Local) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

            glm::mat4 GizmoModel = TransformObj.GetMatrix();
            ImGuizmo::Manipulate(glm::value_ptr(View), glm::value_ptr(Projection), Operation, Mode, glm::value_ptr(GizmoModel));

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 NewPos, NewScale, Skew;
                glm::quat NewRot;
                glm::vec4 Perspective;
                glm::decompose(GizmoModel, NewScale, NewRot, NewPos, Skew, Perspective);

                TransformObj.Position = NewPos;
                TransformObj.Rotation = NewRot;
                TransformObj.Scale = NewScale;
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
