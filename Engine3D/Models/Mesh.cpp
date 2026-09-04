#include "Mesh.h"

Mesh::Mesh(const std::vector<FVertex>& InVertices, const std::vector<GLuint>& InIndices)
    : Vertices(InVertices),
      Indices(InIndices)
{
    SetupMesh();
}

Mesh::~Mesh()
{
    // Vertex arrays and buffers live in separate name spaces,
    // so each kind has to go back through its own deleter
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::Draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Everything bound from here on is recorded into the VAO
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(FVertex), Vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), Indices.data(), GL_STATIC_DRAW);

    // sizeof(FVertex) is the stride and offsetof locates each field on its own,
    // so adding a field to FVertex keeps every offset correct without edits here

    // Position (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), reinterpret_cast<void*>(offsetof(FVertex, Position)));

    // Normal (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), reinterpret_cast<void*>(offsetof(FVertex, Normal)));

    // TexCoords (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FVertex), reinterpret_cast<void*>(offsetof(FVertex, TexCoords)));

    glBindVertexArray(0);
}
