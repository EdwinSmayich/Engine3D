// GLAD must be included BEFORE GLFW - it provides the OpenGL headers.
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../../Textures/stb_image.h"

constexpr GLint WIDTH_SCREEN = 1200;
constexpr GLint HEIGHT_SCREEN = 1000;

glm::mat4 Projection;

static void FrameBufferSizeCallback(GLFWwindow* InWindow, int InWidth, int InHeight)
{
    glViewport(0, 0, InWidth, InHeight);

    if (InHeight == 0)
    {
        InHeight = 1;
    }

    const GLfloat Aspect = static_cast<GLfloat>(InWidth) / static_cast<GLfloat>(InHeight);
    Projection = glm::perspective(glm::radians(90.0f), Aspect, 0.1f, 100.0f);
}

inline const GLvoid* BufferOffset(size_t InBytes)
{
    return reinterpret_cast<GLvoid*>(InBytes);
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

    int FbWidth = 0, FbHeight = 0;
    glfwGetFramebufferSize(Window, &FbWidth, &FbHeight);
    FrameBufferSizeCallback(Window, FbWidth, FbHeight);

    glEnable(GL_DEPTH_TEST);

    GLfloat Vertices[] = {
        // clang-format off
        // Red facet
        // Position           // Color            // UV
        -1.0f, -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
        1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
  
        // Green facet 
        // Position           // Color            // UV
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         
        // Blue facet 
        // Position           // Color             // UV
        1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        
        // Yellow facet  
        // Position           // Color            // UV
        -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        
        // Magenta facet  
        // Position           // Color            // UV
        -1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,    1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        
        // White facet  
        // Position           // Color            // UV
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), BufferOffset(0));
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), BufferOffset(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), BufferOffset(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);

    // Create texture object
    GLuint Texture = 0;
    glGenTextures(1, &Texture);

    // Activate texture unit 0 and bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    // Texture wrapping mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture from file
    GLint Width = 0;
    GLint Height = 0;
    GLint nrChannels = 0;
    GLubyte* TextureData = stbi_load(TEXTURE_DIR "/Guy.jpg", &Width, &Height, &nrChannels, 0);

    if (TextureData)
    {
        // Upload texture to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }
    stbi_image_free(TextureData);

    // Create shader program
    Shader Shader(SHADER_DIR "/3.3.Shader.vs", SHADER_DIR "/3.3.Shader.fs");

    // Camera view matrix
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    while (!glfwWindowShouldClose(Window))
    {
        if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(Window, GLFW_TRUE);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLfloat Time = static_cast<GLfloat>(glfwGetTime());
        GLfloat Angle = Time * (360.0f / 4.0f);

        // Calculate object movement
        GLfloat X = glm::cos(Time) * 2.0f;
        GLfloat Y = glm::sin(Time) * 2.0f;

        // Build model matrix
        glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(X, Y, 0.0f));
        Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(0.5f, 1.0f, 0.0f));

        // Final transformation matrix
        glm::mat4 MVP = Projection * View * Model;

        // Bind shader and update uniforms
        Shader.Use();
        Shader.SetInt("uTexture", 0);
        Shader.SetMat4("uMVP", MVP);

        GLfloat ColorValue = (glm::cos(Time) * 0.5f) + 0.5f;
        Shader.SetVec4("OurColor", glm::vec4(glm::vec3(ColorValue), 1.0f));

        // Bind texture and draw cube
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &Texture);

    glfwTerminate();
    return 0;
}
