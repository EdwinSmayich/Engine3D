// GLAD must be included BEFORE GLFW - it provides the OpenGL headers.
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

constexpr GLint WIDTH_SCREEN = 1200;
constexpr GLint HEIGHT_SCREEN = 1000;

inline const void* BufferOffset(size_t InBytes)
{
    return reinterpret_cast<void*>(InBytes);
}

const GLchar* VertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
    vertexColor = aColor;
}
)";

const GLchar* FragmentSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0f);
}
)";

GLuint CompileShader(GLenum InType, const GLchar* InSource)
{
    GLuint Shader = glCreateShader(InType);
    glShaderSource(Shader, 1, &InSource, nullptr);
    glCompileShader(Shader);
    
    GLint Success = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);

    if (!Success)
    {
        char Log[512];
        glGetShaderInfoLog(Shader, 512, nullptr, Log);
        std::cerr << "Shader compile error:\n" << Log << "\n";
    }
    
    return Shader;
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

    float Vertices[] =
    {
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    
    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BufferOffset(0));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BufferOffset(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    GLuint VertexShader = CompileShader(GL_VERTEX_SHADER, VertexSource);
    GLuint FragmentShader = CompileShader(GL_FRAGMENT_SHADER, FragmentSource);
    
    GLuint Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);
    
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    while (!glfwWindowShouldClose(Window))
    {
        if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(Window, GLFW_TRUE);
        }

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(Program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    
    glfwTerminate();
    return 0;
}
