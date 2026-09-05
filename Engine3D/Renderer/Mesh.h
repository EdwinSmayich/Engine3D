#pragma once
#include "glad/gl.h"

#include <cstdint>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class FShader;

// One vertex exactly as it is stored in the vertex buffer.
// The fields sit back to back in memory, so a std::vector<FVertex> is byte for
// byte the interleaved float array that OpenGL expects - no conversion needed.
struct FVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

enum class ETextureType : std::uint8_t
{
    ETT_Diffuse,
    ETT_Specular,
    ETT_Emission,
};

struct FTexture
{
    GLuint Id = 0;
    ETextureType Type = ETextureType::ETT_Diffuse;
    std::string Path; // Where it was loaded from - lets the loader skip files it already uploaded
};

// A single chunk of geometry that owns its own VAO/VBO/EBO.
// A model is built out of several of these, usually one per material.
class FMesh
{
public:
    FMesh(std::vector<FVertex> InVertices, std::vector<GLuint> InIndices, std::vector<FTexture> InTextures);
    FMesh(const FMesh&) = delete;
    FMesh& operator=(const FMesh&) = delete;
    FMesh(FMesh&& InOther) noexcept;
    FMesh& operator=(FMesh&& InOther) noexcept;

    ~FMesh();

    void Draw(const FShader& InShader) const;

    static const GLchar* GetUniformName(ETextureType InType);

private:
    // Uploads the geometry to the GPU and describes how to read it back
    void SetupMesh();

    std::vector<FVertex> Vertices;
    std::vector<GLuint> Indices;
    std::vector<FTexture> Textures;

    GLuint VAO = 0; // Remembers the attribute layout and which buffers to read from
    GLuint VBO = 0; // Vertex data
    GLuint EBO = 0; // Index data
};
