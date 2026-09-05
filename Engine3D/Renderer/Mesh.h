#pragma once
#include "glad/gl.h"

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class AShader;

// One vertex exactly as it is stored in the vertex buffer.
// The fields sit back to back in memory, so a std::vector<FVertex> is byte for
// byte the interleaved float array that OpenGL expects - no conversion needed.
struct FVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct FTexture
{
    GLuint Id;
    std::string Type;
    std::string Path;
};

// A single chunk of geometry that owns its own VAO/VBO/EBO.
// A model is built out of several of these, usually one per material.
class AMesh
{
public:
    AMesh(std::vector<FVertex> InVertices, std::vector<GLuint> InIndices /*,std::vector<FTexture> InTextures*/);
    AMesh(const AMesh&) = delete;
    AMesh& operator=(const AMesh&) = delete;
    AMesh(AMesh&& InOther) noexcept;
    AMesh& operator=(AMesh&& InOther) noexcept;

    ~AMesh();

    void Draw(const AShader& InShader) const;

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
