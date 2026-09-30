#pragma once
#include "glad/gl.h"

#include <cstdint>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class FShader;

enum class ETextureType : std::uint8_t
{
    ETT_Diffuse,
    ETT_Specular,
    ETT_Emission,
    ETT_None
};

struct FVertex
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Normal = glm::vec3(0.0f);
    glm::vec2 TexCoords = glm::vec2(0.0f);
};

struct FTexture
{
    GLuint Id = 0;
    ETextureType Type = ETextureType::ETT_None;
    std::string Path = "";
};

// A single chunk of geometry that owns its own VAO/VBO/EBO.
// A model is built out of several of these, usually one per material.
class FMesh
{
public:
    FMesh(std::vector<FVertex> InVertices, std::vector<GLuint> InIndices, std::vector<FTexture> InTextures);

    FMesh(const FMesh& InOther) = delete;
    FMesh& operator=(const FMesh& InOther) = delete;
    FMesh(FMesh&& InOther) noexcept;
    FMesh& operator=(FMesh&& InOther) noexcept;

    ~FMesh();

    void Draw(const FShader& InShader) const;

private:
    void SetupMesh();
    static const GLchar* GetUniformName(ETextureType InType);

    std::vector<FVertex> Vertices;
    std::vector<GLuint> Indices;
    std::vector<FTexture> Textures;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
};
