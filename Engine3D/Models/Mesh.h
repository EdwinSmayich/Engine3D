#pragma once
#include "glad/gl.h"

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

// One vertex exactly as it is stored in the vertex buffer.
// The fields sit back to back in memory, so a std::vector<FVertex> is byte for
// byte the interleaved float array that OpenGL expects - no conversion needed.
struct FVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// A single chunk of geometry that owns its own VAO/VBO/EBO.
// A model is built out of several of these, usually one per material.
class Mesh
{
public:
    Mesh(const std::vector<FVertex>& InVertices, const std::vector<GLuint>& InIndices);
    ~Mesh();

    void Draw() const;

private:
    // Uploads the geometry to the GPU and describes how to read it back
    void SetupMesh();

    std::vector<FVertex> Vertices;
    std::vector<GLuint> Indices;

    GLuint VAO = 0; // Remembers the attribute layout and which buffers to read from
    GLuint VBO = 0; // Vertex data
    GLuint EBO = 0; // Index data
};
