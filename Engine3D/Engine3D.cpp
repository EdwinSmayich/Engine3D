// GLAD must be included BEFORE GLFW - it provides the OpenGL headers.
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../../Textures/stb_image.h"
#include "Camera/Camera.h"

// Screen settings
constexpr GLint WIDTH_SCREEN = 1200;
constexpr GLint HEIGHT_SCREEN = 1000;

// Camera
Camera MainCamera(glm::vec3(0.0f, 0.0f, 5.0f));
GLboolean bFirstMouse = true;
GLfloat LastX = static_cast<GLfloat>(WIDTH_SCREEN) * 0.5f;
GLfloat LastY = static_cast<GLfloat>(HEIGHT_SCREEN) * 0.5f;

// MVP settings
glm::mat4 Projection;

// Stores how much we're seeing of either texture
GLfloat MixValue = 0.2f;

static void FrameBufferSizeCallback(GLFWwindow*, int InWidth, int InHeight)
{
    glViewport(0, 0, InWidth, InHeight);
}

static void MouseCallBack(GLFWwindow*, GLdouble InPosX, GLdouble InPosY)
{
    if (bFirstMouse)
    {
        LastX = static_cast<float>(InPosX);
        LastY = static_cast<float>(InPosY);
        bFirstMouse = false;
    }

    GLfloat OffsetX = static_cast<float>(InPosX) - LastX;
    GLfloat OffsetY = LastY - static_cast<float>(InPosY); // Reversed since y-coordinates range from bottom to top

    LastX = static_cast<float>(InPosX);
    LastY = static_cast<float>(InPosY);

    MainCamera.ProcessMouseMovement(OffsetX, OffsetY, GL_TRUE);
}

static void ScrollCallBack(GLFWwindow*, GLdouble, GLdouble InOffsetY)
{
    MainCamera.ProcessMouseScroll(InOffsetY);
}

inline const GLvoid* BufferOffset(size_t InBytes)
{
    return reinterpret_cast<GLvoid*>(InBytes);
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react
// accordingly
void ProcessInput(GLFWwindow* InWindow, GLfloat InDeltaTime)
{
    if (glfwGetKey(InWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(InWindow, GLFW_TRUE);
    }

    if (glfwGetKey(InWindow, GLFW_KEY_UP) == GLFW_PRESS)
    {
        MixValue += 2.0f * InDeltaTime;
        MixValue = glm::clamp(MixValue, 0.0f, 1.0f);
    }
    else if (glfwGetKey(InWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        MixValue -= 2.0f * InDeltaTime;
        MixValue = glm::clamp(MixValue, 0.0f, 1.0f);
    }

    // Camera movement
    if (glfwGetKey(InWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        MainCamera.ProcessKeyboard(CameraMovementType::CMT_Forward, InDeltaTime);
    }
    if (glfwGetKey(InWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        MainCamera.ProcessKeyboard(CameraMovementType::CMT_Backward, InDeltaTime);
    }
    if (glfwGetKey(InWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        MainCamera.ProcessKeyboard(CameraMovementType::CMT_Right, InDeltaTime);
    }
    if (glfwGetKey(InWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        MainCamera.ProcessKeyboard(CameraMovementType::CMT_Left, InDeltaTime);
    }
}

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

    GLint FbWidth = 0, FbHeight = 0;
    glfwGetFramebufferSize(Window, &FbWidth, &FbHeight);
    FrameBufferSizeCallback(Window, FbWidth, FbHeight);

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLfloat Vertices[] = {
        // clang-format off
        // Red facet
        // Position           // Color            // UV         // Normal
        -1.0f, -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
  
        // Green facet 
        // Position           // Color            // UV         // Normal
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, -1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
         
        // Blue facet 
        // Position           // Color             // UV        // Normal
        1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
        
        // Yellow facet  
        // Position           // Color            // UV         // Normal
        -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
        
        // Magenta facet  
        // Position           // Color            // UV         // Normal
        -1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        
        // White facet  
        // Position           // Color            // UV         // Normal
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
        // clang-format on
    };

    GLuint Indices[] = {
        // clang-format off
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
        // clang-format on
    };

    // clang-format off
    glm::vec3 CubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 5.0f,  7.0f, -15.0f), 
        glm::vec3(-4.5f, -4.2f, -2.5f),  
        glm::vec3(-7.8f, -4.0f, -12.3f),  
        glm::vec3( 5.4f, -2.4f, -3.5f),  
        glm::vec3(-4.7f,  5.0f, -7.5f),  
        glm::vec3( 2.3f, -4.0f, -2.5f),  
        glm::vec3( 4.5f,  4.0f, -2.5f), 
        glm::vec3( 4.5f,  1.2f, -1.5f), 
        glm::vec3(-4.3f,  2.0f, -1.5f)  
    };
    // clang-format on

    // Vertex Array
    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex Buffer
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), BufferOffset(0));
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), BufferOffset(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), BufferOffset(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), BufferOffset(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    // Unbind VAO
    glBindVertexArray(0);

    // Create texture object
    // Texture 1
    GLuint Texture1 = 0;
    glGenTextures(1, &Texture1);

    // Activate textures unit and bind textures
    glBindTexture(GL_TEXTURE_2D, Texture1);

    // Texture wrapping mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load textures from file
    GLint Width = 0;
    GLint Height = 0;
    GLint nrChannels = 0;
    GLubyte* TextureData1 = stbi_load(TEXTURE_DIR "/Guy.jpg", &Width, &Height, &nrChannels, 0);

    if (TextureData1)
    {
        // Upload texture to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }
    stbi_image_free(TextureData1);

    // Texture 2
    GLuint Texture2 = 0;
    glGenTextures(1, &Texture2);
    glBindTexture(GL_TEXTURE_2D, Texture2);

    // Texture wrapping mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLubyte* TextureData2 = stbi_load(TEXTURE_DIR "/AwesomeFace.png", &Width, &Height, &nrChannels, 0);
    if (TextureData2)
    {
        // Upload texture to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }
    stbi_image_free(TextureData2);

    // Create shader program
    Shader Shader(SHADER_DIR "/3.3.Shader.vs", SHADER_DIR "/3.3.Shader.fs");

    GLfloat LastFrame = 0.0f;

    // Bind shader and texture uniforms
    Shader.Use();
    Shader.SetInt("uTexture1", 0);
    Shader.SetInt("uTexture2", 1);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    while (!glfwWindowShouldClose(Window))
    {
        GLfloat CurrentFrame = static_cast<GLfloat>(glfwGetTime());
        GLfloat DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        ProcessInput(Window, DeltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update uniforms
        Shader.SetFloat("uMixValue", MixValue);

        // Bind textures and draw cube
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Texture2);

        // Camera view matrix
        constexpr GLfloat Aspect = static_cast<GLfloat>(WIDTH_SCREEN) / static_cast<GLfloat>(HEIGHT_SCREEN);
        const GLfloat FOV = MainCamera.GetZoom();
        Projection = glm::perspective(glm::radians(FOV), Aspect, 0.1f, 100.0f);
        Shader.SetMat4("uProjection", Projection);

        glm::mat4 View = MainCamera.GetViewMatrix();
        Shader.SetMat4("uView", View);

        // Build model matrix
        glBindVertexArray(VAO);
        for (int i = 0; i < 10; ++i)
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, CubePositions[i]);

            GLfloat Angle = CurrentFrame * glm::radians(360.0f);
            Model = glm::rotate(Model, Angle, glm::vec3(0.0f, 1.0f, 0.0f));

            Shader.SetMat4("uModel", Model);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &Texture1);
    glDeleteTextures(1, &Texture2);

    glfwTerminate();
    return 0;
}
