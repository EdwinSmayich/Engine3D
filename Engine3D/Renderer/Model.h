#pragma once
#include <string>
#include <vector>
#include "assimp/material.h"

class FShader;
class FMesh;
struct aiScene;
struct aiNode;
struct aiMesh;
struct FTexture;
struct aiMaterial;

class FModel
{
public:
    explicit FModel(const std::string& InPath);

    void Draw(const FShader& InShader) const;

private:
    void LoadModel(const std::string& InPath);
    void ProcessNode(const aiNode* InNode, const aiScene* InScene);
    FMesh ProcessMesh(const aiMesh* InMesh, const aiScene* InScene);
    // std::vector<FTexture> LoadMaterialTexture(aiMaterial* InMaterial, aiTextureType InType, const std::string& InTypeName);

    std::vector<FMesh> Meshes;
    std::string Directory;
};
