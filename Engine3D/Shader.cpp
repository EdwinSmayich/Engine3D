#include "Shader.h"

#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const GLchar* InVertexPath, const GLchar* InFragmentPath)
{
    // 1. Retrieve the vertex/fragment source code from FilePath
    std::string VertexCode, FragmentCode;
    std::ifstream VertexShaderFile, FragmentShaderFile;

    VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Open files
        VertexShaderFile.open(InVertexPath);
        FragmentShaderFile.open(InFragmentPath);

        std::stringstream VertexStream, FragmentStream;

        // Read file's buffer contents into streams
        VertexStream << VertexShaderFile.rdbuf();
        FragmentStream << FragmentShaderFile.rdbuf();

        // Convert stream into string
        VertexCode = VertexStream.str();
        FragmentCode = FragmentStream.str();

        // Close file handlers
        VertexShaderFile.close();
        FragmentShaderFile.close();
    }
    catch (std::ifstream::failure& Error)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << Error.what() << std::endl;
    }

    const GLchar* VertexShaderCode = VertexCode.c_str();
    const GLchar* FragmentShaderCode = FragmentCode.c_str();

    // 2. compile shaders
    // Vertex shader
    GLuint Vertex = 0;
    Vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Vertex, 1, &VertexShaderCode, nullptr);
    glCompileShader(Vertex);
    CheckCompileErrors(Vertex, "VERTEX");

    // Fragment shader
    GLuint Fragment = 0;
    Fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Fragment, 1, &FragmentShaderCode, nullptr);
    glCompileShader(Fragment);
    CheckCompileErrors(Fragment, "FRAGMENT");

    // Shader program
    ID = glCreateProgram();
    glAttachShader(ID, Vertex);
    glAttachShader(ID, Fragment);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    glDeleteShader(Vertex);
    glDeleteShader(Fragment);
}

Shader::~Shader()
{
    if (ID != 0)
    {
        glDeleteProgram(ID);
    }
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetBool(const std::string& InName, GLboolean InValue) const
{
    glUniform1i(glGetUniformLocation(ID, InName.c_str()), static_cast<int>(InValue));
}

void Shader::SetInt(const std::string& InName, GLint InValue) const
{
    glUniform1i(glGetUniformLocation(ID, InName.c_str()), InValue);
}

void Shader::SetFloat(const std::string& InName, GLfloat InValue) const
{
    glUniform1f(glGetUniformLocation(ID, InName.c_str()), InValue);
}

void Shader::SetVec4(const std::string& InName, const glm::vec4& InValue) const
{
    glUniform4f(glGetUniformLocation(ID, InName.c_str()), InValue.x, InValue.y, InValue.z, InValue.w);
}

void Shader::SetMat4(const std::string& InName, const glm::mat4& InValue) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, InName.c_str()), 1, GL_FALSE, glm::value_ptr(InValue));
}

GLuint Shader::GetShaderProgram() const
{
    return ID;
}

void Shader::CheckCompileErrors(GLuint InShader, const std::string& InType) const
{
    GLint Success = 0;
    GLchar InfoLog[1024] = {};
    if (InType != "PROGRAM")
    {
        glGetShaderiv(InShader, GL_COMPILE_STATUS, &Success);
        if (!Success)
        {
            glGetShaderInfoLog(InShader, 1024, nullptr, InfoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << InType << "\n"
                      << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(InShader, GL_LINK_STATUS, &Success);
        if (!Success)
        {
            glGetProgramInfoLog(InShader, 1024, nullptr, InfoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << InType << "\n"
                      << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
