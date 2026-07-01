// GLAD must be included BEFORE GLFW - it provides the OpenGL headers.
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../Textures/stb_image.h"
#include "Shader.h"

constexpr GLint WIDTH_SCREEN = 1000;
constexpr GLint HEIGHT_SCREEN = 1000;

// Called by the driver (when a debug context is active) whenever OpenGL has
// something to report. Turns the raw report into readable console output.
void APIENTRY OpenGLDebugCallback(
    GLenum Source,
    GLenum Type,
    GLuint Id,
    GLenum Severity,
    GLsizei Length,
    const GLchar* Message,
    const void* UserParam)
{
    std::cerr << "\n========== OpenGL ==========\n" << "ID: " << Id << '\n' << "Severity: ";

    switch (Severity)
    {
    case GL_DEBUG_SEVERITY_HIGH: std::cerr << "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW: std::cerr << "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "NOTIFICATION";
        break;
    }

    std::cerr << "\nMessage: " << Message << "\n============================\n";
}

// Keeps the OpenGL viewport in sync with the window size when it is resized.
static void FramebufferSizeCallback(GLFWwindow* InWindow, GLint InWidth, GLint InHeight)
{
    glViewport(0, 0, InWidth, InHeight);
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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* Window = glfwCreateWindow(WIDTH_SCREEN, HEIGHT_SCREEN, "Engine3D", nullptr, nullptr);
    if (!Window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    // Load OpenGL function pointers via GLAD.
    GLint Version = gladLoadGL(glfwGetProcAddress);
    if (!Version)
    {
        std::cerr << "Failed to initialize OpenGL context (GLAD)\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(Version) << "." << GLAD_VERSION_MINOR(Version) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

    // --- Optional debug output (only if the context was created in debug mode) ---
    GLint Flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &Flags);
    if (Flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLDebugCallback, nullptr);
        glDebugMessageControl(
            GL_DEBUG_SOURCE_API,
            GL_DEBUG_TYPE_OTHER,
            GL_DEBUG_SEVERITY_NOTIFICATION,
            0,
            nullptr,
            GL_FALSE);
    }

    // Shaders now carry the color per vertex, so one program drives both triangles.
    // SHADER_DIR (set by CMake) is the absolute Shaders/ path; the adjacent string
    // literals are concatenated at compile time into the full file path.
    Shader OurShaderProgram(SHADER_DIR "/3.3.Shader.vs", SHADER_DIR "/3.3.Shader.fs");

    // --- Texture lesson (texture triangle) ---
    GLfloat Vertices[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f,   0.0f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // top right
        0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f // top left 
    };

    GLuint Incides[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    GLuint TextureVBO = 0;
    GLuint TextureVAO = 0;
    GLuint TextureEBO = 0;
    glGenBuffers(1, &TextureVBO);
    glGenVertexArrays(1, &TextureVAO);
    glGenBuffers(1, &TextureEBO);

    glBindVertexArray(TextureVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TextureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TextureEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Incides), Incides, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Texture attribute
    GLuint Texture = 0;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint Width = 0;
    GLint Height = 0;
    GLint HrChannels = 0;
    unsigned char* Data = stbi_load(TEXTURE_DIR "/Container.jpg", &Width, &Height, &HrChannels, 0);

    if (Data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(Data);

    // --- Render loop ---------------------------------------------------------
    while (!glfwWindowShouldClose(Window))
    {
        if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(Window, GLFW_TRUE);
        }

        glClearColor(0.10f, 0.15f, 0.20f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, Texture);

        // One shader program for both triangles; the color comes from the vertices.
        OurShaderProgram.Use();

        // GLfloat OffsetX = 0.7f;
        // OurShaderProgram.SetFloat("OffsetX", OffsetX);

        //OurShaderProgram.SetInt("OurTexture", 0);
        glBindVertexArray(TextureVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    // --- Cleanup -------------------------------------------------------------
    glDeleteBuffers(1, &TextureVBO);
    glDeleteVertexArrays(1, &TextureVAO);
    glDeleteBuffers(1, &TextureEBO);
    glDeleteTextures(1, &Texture);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}
