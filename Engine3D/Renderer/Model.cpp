#include "Model.h"

#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <iostream>

FModel::FModel(const std::string& InPath)
{
    LoadModel(InPath);
}

void FModel::Draw(const FShader& InShader) const
{
    for (const FMesh& Mesh : Meshes)
    {
        Mesh.Draw(InShader);
    }
}

void FModel::LoadModel(const std::string& InPath)
{
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(InPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

    if (!Scene || (Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !Scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP: " << Importer.GetErrorString() << "\n";
        return;
    }

    // Textures are named relative to the model file, so keep the folder it came from
    Directory = InPath.substr(0, InPath.find_last_of("/\\"));
    ProcessNode(Scene->mRootNode, Scene);
}

void FModel::ProcessNode(const aiNode* InNode, const aiScene* InScene)
{
    for (GLuint i = 0; i < InNode->mNumMeshes; ++i)
    {
        aiMesh* Mesh = InScene->mMeshes[InNode->mMeshes[i]];
        Meshes.push_back(ProcessMesh(Mesh, InScene));
    }

    for (GLuint i = 0; i < InNode->mNumChildren; ++i)
    {
        ProcessNode(InNode->mChildren[i], InScene);
    }
}

FMesh FModel::ProcessMesh(const aiMesh* InMesh, const aiScene* InScene)
{
    std::vector<FVertex> Vertices;
    std::vector<GLuint> Indices;
    // std::vector<FTexture> Textures;

    // Both counts are known up front, so allocate once instead of growing
    Vertices.reserve(InMesh->mNumVertices);
    Indices.reserve(static_cast<size_t>(InMesh->mNumFaces) * 3); // Triangulate guarantees three per face

    for (GLuint i = 0; i < InMesh->mNumVertices; ++i)
    {
        FVertex Vertex;

        // We declare a placeholder vector since assimp uses its own vector class
        // that doesn't directly convert to glm's vec3 class
        // so we transfer the data to this placeholder glm::vec3 first.
        glm::vec3 Vec3;

        // Position
        Vec3.x = InMesh->mVertices[i].x;
        Vec3.y = InMesh->mVertices[i].y;
        Vec3.z = InMesh->mVertices[i].z;
        Vertex.Position = Vec3;

        // Normal
        if (InMesh->HasNormals())
        {
            Vec3.x = InMesh->mNormals[i].x;
            Vec3.y = InMesh->mNormals[i].y;
            Vec3.z = InMesh->mNormals[i].z;
            Vertex.Normal = Vec3;
        }

        // Texture coordinates
        if (InMesh->HasTextureCoords(0))
        {
            glm::vec2 Vec2;
            Vec2.x = InMesh->mTextureCoords[0][i].x;
            Vec2.y = InMesh->mTextureCoords[0][i].y;
            Vertex.TexCoords = Vec2;
        }
        else
        {
            Vertex.TexCoords = glm::vec2(0.0f);
        }

        Vertices.push_back(Vertex);
    }

    for (GLuint i = 0; i < InMesh->mNumFaces; ++i)
    {
        const aiFace& Face = InMesh->mFaces[i];
        for (GLuint j = 0; j < Face.mNumIndices; ++j)
        {
            Indices.push_back(Face.mIndices[j]);
        }
    }

    // Process materials
    // aiMaterial* Material = InScene->mMaterials[InMesh->mMaterialIndex];

    return FMesh(std::move(Vertices), std::move(Indices), {});
}

// std::vector<FTexture> FModel::LoadMaterialTexture(aiMaterial* InMaterial, aiTextureType InType, const std::string& InTypeName)
// {
//
// }