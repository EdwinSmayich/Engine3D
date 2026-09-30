#include "Model.h"

#include <iostream>
#include <algorithm>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Mesh.h"
#include "../../Textures/Texture.h"

FModel::FModel(const std::string& InPath)
{
    LoadModel(InPath);
}

void FModel::Draw(const FShader& InShader) const
{
    for (size_t i = 0; i < Meshes.size(); ++i)
    {
        Meshes[i].Draw(InShader);
    }
}

void FModel::LoadModel(const std::string& InPath)
{
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(InPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    if (!Scene || (Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !Scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP " << Importer.GetErrorString() << "\n";
        return;
    }

    Meshes.reserve(Scene->mNumMeshes);
    Directory = InPath.substr(0, InPath.find_last_of("/\\"));

    ProcessNode(Scene->mRootNode, Scene);
}

void FModel::ProcessNode(const aiNode* InNode, const aiScene* InScene)
{
    for (size_t i = 0; i < InNode->mNumMeshes; ++i)
    {
        aiMesh* Mesh = InScene->mMeshes[InNode->mMeshes[i]];
        Meshes.push_back(ProcessMesh(Mesh, InScene));
    }

    for (size_t i = 0; i < InNode->mNumChildren; ++i)
    {
        ProcessNode(InNode->mChildren[i], InScene);
    }
}

FMesh FModel::ProcessMesh(const aiMesh* InMesh, const aiScene* InScene)
{
    std::vector<FVertex> Vertices;
    std::vector<GLuint> Indices;
    std::vector<FTexture> Textures;

    Vertices.reserve(InMesh->mNumVertices);
    Indices.reserve(InMesh->mNumFaces * 3);

    for (GLuint i = 0; i < InMesh->mNumVertices; ++i)
    {
        FVertex Vertex;

        // Position
        Vertex.Position.x = InMesh->mVertices[i].x;
        Vertex.Position.y = InMesh->mVertices[i].y;
        Vertex.Position.z = InMesh->mVertices[i].z;

        // Normal
        if (InMesh->HasNormals())
        {
            Vertex.Normal.x = InMesh->mNormals[i].x;
            Vertex.Normal.y = InMesh->mNormals[i].y;
            Vertex.Normal.z = InMesh->mNormals[i].z;
        }

        // Texture coords
        if (InMesh->mTextureCoords[0])
        {
            Vertex.TexCoords.x = InMesh->mTextureCoords[0][i].x;
            Vertex.TexCoords.y = InMesh->mTextureCoords[0][i].y;
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
        for (size_t j = 0; j < Face.mNumIndices; ++j)
        {
            Indices.push_back(Face.mIndices[j]);
        }
    }

    if (InMesh->mMaterialIndex < InScene->mNumMaterials)
    {
        aiMaterial* Material = InScene->mMaterials[InMesh->mMaterialIndex];

        std::vector<FTexture> DiffuseMap = LoadMaterialTextures(Material, ETextureType::ETT_Diffuse);
        Textures.insert(Textures.end(), DiffuseMap.begin(), DiffuseMap.end());

        std::vector<FTexture> SpecularMap = LoadMaterialTextures(Material, ETextureType::ETT_Specular);
        Textures.insert(Textures.end(), SpecularMap.begin(), SpecularMap.end());
    }

    return FMesh(std::move(Vertices), std::move(Indices), std::move(Textures));
}

static aiTextureType ToAssimpType(ETextureType InType)
{
    switch (InType)
    {
        case ETextureType::ETT_Diffuse:
        {
            return aiTextureType_DIFFUSE;
        }
        case ETextureType::ETT_Specular:
        {
            return aiTextureType_SPECULAR;
        }
        case ETextureType::ETT_Emission:
        {
            return aiTextureType_EMISSIVE;
        }
        case ETextureType::ETT_None:
        {
            return aiTextureType_NONE;
        }
    }

    return aiTextureType_NONE;
}

std::vector<FTexture> FModel::LoadMaterialTextures(const aiMaterial* InMaterial, ETextureType InType)
{
    std::vector<FTexture> Result;
    aiTextureType AiType = ToAssimpType(InType);
    GLuint Count = InMaterial->GetTextureCount(AiType);

    if (Count > 1)
    {
        std::cerr << "Our design keeps one texture per type " << Directory << "\n";
    }

    for (GLuint i = 0; i < Count; ++i)
    {
        aiString Str;
        InMaterial->GetTexture(AiType, i, &Str);
        std::string FullPath = Directory + "/" + Str.C_Str();

        // clang-format off
        auto It = std::find_if(
            LoadedTextures.begin(),
            LoadedTextures.end(),
            [&](const FTexture& InTexture)
            {
                return InTexture.Path == FullPath;
            });
        // clang-format on

        if (It != LoadedTextures.end())
        {
            Result.push_back(*It);
        }
        else
        {
            FTexture Tex;
            Tex.Id = Texture::LoadTexture(FullPath.c_str());
            Tex.Type = InType;
            Tex.Path = FullPath;

            LoadedTextures.push_back(Tex);
            Result.push_back(Tex);
        }
    }

    return Result;
}
