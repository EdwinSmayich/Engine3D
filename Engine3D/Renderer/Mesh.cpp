#include "Mesh.h"
#include "Shader.h"

FMesh::FMesh(std::vector<FVertex> InVertices, std::vector<GLuint> InIndices, std::vector<FTexture> InTextures)
    : Vertices(std::move(InVertices)),
      Indices(std::move(InIndices)),
      Textures(std::move(InTextures))
{
    SetupMesh();
}

FMesh::FMesh(FMesh&& InOther) noexcept
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

FMesh::~FMesh()
{
    // Vertex arrays and buffers live in separate name spaces,
    // so each kind has to go back through its own deleter
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

FMesh& FMesh::operator=(FMesh&& InOther) noexcept
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

void FMesh::Draw(const FShader& InShader) const
{
    for (GLint Unit = 0; Unit < static_cast<GLint>(Textures.size()); ++Unit)
    {
        glActiveTexture(GL_TEXTURE0 + Unit);
        glBindTexture(GL_TEXTURE_2D, Textures[Unit].Id);
        InShader.SetInt(GetUniformName(Textures[Unit].Type), Unit);
    }

    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

const GLchar* FMesh::GetUniformName(ETextureType InType)
{
    switch (InType)
    {
        case ETextureType::ETT_Diffuse:
        {
            return "uMaterial.Diffuse";
        }
        case ETextureType::ETT_Specular:
        {
            return "uMaterial.Specular";
        }
        case ETextureType::ETT_Emission:
        {
            return "uMaterial.Emission";
        }
    }

    return "";
}

void FMesh::SetupMesh()
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
