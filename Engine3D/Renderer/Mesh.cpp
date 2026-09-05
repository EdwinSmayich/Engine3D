#include "Mesh.h"

#include "Shader.h"

AMesh::AMesh(std::vector<FVertex> InVertices, std::vector<GLuint> InIndices /*, std::vector<FTexture> InTextures*/)
    : Vertices(std::move(InVertices)),
      Indices(std::move(InIndices))
// Textures(std::move(InTextures))
{
    SetupMesh();
}

AMesh::~AMesh()
{
    // Vertex arrays and buffers live in separate name spaces,
    // so each kind has to go back through its own deleter
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

AMesh::AMesh(AMesh&& InOther) noexcept
    : Vertices(std::move(InOther.Vertices)),
      Indices(std::move(InOther.Indices)),
      Textures(std::move(InOther.Textures)),
      VAO(InOther.VAO),
      VBO(InOther.VBO),
      EBO(InOther.EBO)
{
    // Zero out the source so its destructor has nothing left to free.
    // Deleting name 0 is defined as a no-op, which is why it is the empty handle
    InOther.VAO = 0;
    InOther.VBO = 0;
    InOther.EBO = 0;
}

AMesh& AMesh::operator=(AMesh&& InOther) noexcept
{
    if (this != &InOther)
    {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);

        Vertices = std::move(InOther.Vertices);
        Indices = std::move(InOther.Indices);
        Textures = std::move(InOther.Textures);

        VAO = InOther.VAO;
        VBO = InOther.VBO;
        EBO = InOther.EBO;

        InOther.VAO = 0;
        InOther.VBO = 0;
        InOther.EBO = 0;
    }

    return *this;
}

void AMesh::Draw(const AShader& InShader) const
{
    GLuint DiffuseNr = 1;
    for (GLuint i = 0; i < Textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string Number = std::to_string(DiffuseNr++);
        InShader.SetInt("uMaterial." + Textures[i].Type + Number, i);
        glBindTexture(GL_TEXTURE_2D, Textures[i].Id);
    }

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void AMesh::SetupMesh()
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
