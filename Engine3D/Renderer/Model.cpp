#include "Model.h"

#include <iostream>
#include <algorithm>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Mesh.h"
#include "../../Textures/Texture.h"

// Our texture slot to the one Assimp queries materials by. Kept file local so
// that aiTextureType stays out of Model.h and the header pulls in no Assimp.
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
    // The Importer owns the aiScene and frees it when it goes out of scope, so
    // every conversion to our own types has to finish before this returns
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(InPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

    // A partly read file still comes back as a non-null scene, so the flag has
    // to be checked explicitly or the model loads silently incomplete
    if (!Scene || (Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !Scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP " << Importer.GetErrorString() << "\n";
        return;
    }

    // The node tree only holds indices, so the scene is the one place that
    // knows how many meshes there are in total
    Meshes.reserve(Scene->mNumMeshes);

    // Materials name their textures relative to the model file
    Directory = InPath.substr(0, InPath.find_last_of("/\\"));

    ProcessNode(Scene->mRootNode, Scene);
}

void FModel::ProcessNode(const aiNode* InNode, const aiScene* InScene)
{
    // A node stores indices into the scene's flat mesh array rather than meshes
    // of its own, because several nodes may point at the same geometry
    for (GLuint i = 0; i < InNode->mNumMeshes; ++i)
    {
        const aiMesh* Mesh = InScene->mMeshes[InNode->mMeshes[i]];
        Meshes.push_back(ProcessMesh(Mesh, InScene));
    }

    // The tree has no fixed depth, so each child is handled the same way
    for (GLuint i = 0; i < InNode->mNumChildren; ++i)
    {
        ProcessNode(InNode->mChildren[i], InScene);
    }
}

FMesh FModel::ProcessMesh(const aiMesh* InMesh, const aiScene* InScene)
{
    std::vector<FVertex> Vertices;
    std::vector<GLuint> Indices;
    std::vector<FTexture> Textures;

    // Both sizes are known up front, so allocate once instead of growing
    Vertices.reserve(static_cast<size_t>(InMesh->mNumVertices));
    Indices.reserve(static_cast<size_t>(InMesh->mNumFaces) * 3); // Triangulate guarantees three per face

    for (GLuint i = 0; i < InMesh->mNumVertices; ++i)
    {
        FVertex Vertex;

        // Assimp has its own vector type, so each field is copied across by hand
        Vertex.Position.x = InMesh->mVertices[i].x;
        Vertex.Position.y = InMesh->mVertices[i].y;
        Vertex.Position.z = InMesh->mVertices[i].z;

        if (InMesh->HasNormals())
        {
            Vertex.Normal.x = InMesh->mNormals[i].x;
            Vertex.Normal.y = InMesh->mNormals[i].y;
            Vertex.Normal.z = InMesh->mNormals[i].z;
        }

        // A mesh carries up to eight sets of texture coordinates and may carry
        // none at all, so the first set has to be checked before it is read
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
        for (GLuint j = 0; j < Face.mNumIndices; ++j)
        {
            Indices.push_back(Face.mIndices[j]);
        }
    }

    // Materials live in their own flat array and meshes reference them by index,
    // the same way nodes reference meshes
    if (InMesh->mMaterialIndex < InScene->mNumMaterials)
    {
        const aiMaterial* Material = InScene->mMaterials[InMesh->mMaterialIndex];

        std::vector<FTexture> DiffuseMap = LoadMaterialTextures(Material, ETextureType::ETT_Diffuse);
        Textures.insert(Textures.end(), DiffuseMap.begin(), DiffuseMap.end());

        std::vector<FTexture> SpecularMap = LoadMaterialTextures(Material, ETextureType::ETT_Specular);
        Textures.insert(Textures.end(), SpecularMap.begin(), SpecularMap.end());
    }

    // The Mesh constructor takes its vectors by value so that it can claim them,
    // which only pays off if they are handed over rather than passed as lvalues
    return FMesh(std::move(Vertices), std::move(Indices), std::move(Textures));
}

std::vector<FTexture> FModel::LoadMaterialTextures(const aiMaterial* InMaterial, ETextureType InType)
{
    std::vector<FTexture> Result;
    aiTextureType AiType = ToAssimpType(InType);
    GLuint Count = InMaterial->GetTextureCount(AiType);

    // Draw() binds one uniform per type, so anything past the first is dropped.
    // Say so rather than letting the limit go unnoticed
    if (Count > 1)
    {
        std::cerr << "Our design keeps one texture per type " << Directory << "\n";
    }

    for (GLuint i = 0; i < Count; ++i)
    {
        aiString Str;
        InMaterial->GetTexture(AiType, i, &Str);
        std::string FullPath = Directory + "/" + Str.C_Str();

        // Meshes of one model usually share their textures. A linear scan is
        // enough here: there are tens of them and this runs once, at load
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
