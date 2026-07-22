#pragma once
#include "glm/fwd.hpp"

#include <glad/gl.h>

#include <string>
#include "glm/vec4.hpp"

/**
 * @brief Shader class for working with OpenGL shaders.
 */
class Shader
{
public:
    /**
     * @brief Constructs a shader program by compiling vertex and fragment shaders from files.
     * Constructor generates the shader on the fly.
     *
     * @param InVertexPath Path to the vertex shader.
     * @param InFragmentPath Path to the fragment shader.
     */
    explicit Shader(const GLchar* InVertexPath, const GLchar* InFragmentPath);

    /**
     * @brief Destructor for the Shader class.
     * Cleans up OpenGL shader resources when the object is destroyed.
     */
    ~Shader();

    /**
     * @brief Deleted copy constructor.
     * Prevents copying of shader objects to avoid resource management issues.
     */
    Shader(const Shader&) = delete;

    /**
     * @brief Deleted assignment operator.
     * Prevents assignment of shader objects to avoid resource management issues.
     */
    Shader& operator=(const Shader&) = delete;

    /**
     * @brief Activates the shader program.
     */
    void Use() const;

    /**
     * @brief Sets a boolean value for a shader parameter.
     *
     * @param InName Name of the parameter.
     * @param InValue Value of the parameter.
     */
    void SetBool(const std::string& InName, GLboolean InValue) const;

    /**
     * @brief Sets an integer value for a shader parameter.
     *
     * @param InName Name of the parameter.
     * @param InValue Value of the parameter.
     */
    void SetInt(const std::string& InName, GLint InValue) const;

    /**
     * @brief Sets a float value for a shader parameter.
     *
     * @param InName Name of the parameter.
     * @param InValue Value of the parameter.
     */
    void SetFloat(const std::string& InName, GLfloat InValue) const;

    /**
     * @brief Sets a vec4 value for a shader uniform.
     *
     * @param InName Name of the uniform variable.
     * @param InValue Value to be assigned.
     */
    void SetVec4(const std::string& InName, const glm::vec4& InValue) const;

    /**
     * @brief Sets a 4x4 matrix value for a shader uniform.
     *
     * Used mainly for transformation matrices such as
     * Model, View, Projection or MVP matrices.
     *
     * @param InName Name of the uniform variable.
     * @param InValue Matrix to be assigned.
     */
    void SetMat4(const std::string& InName, const glm::mat4& InValue) const;

    /**
     * @brief Returns the OpenGL shader program ID.
     *
     * @return OpenGL program handle.
     */
    GLuint GetShaderProgram() const;

private:
    /**
     * @brief Checks shader compilation or program linking errors.
     *
     * @param InShader Shader or program ID.
     * @param InType Type string ("VERTEX", "FRAGMENT", or "PROGRAM").
     */
    void CheckCompileErrors(GLuint InShader, const std::string& InType) const;

private:
    GLuint ID = 0;
};
