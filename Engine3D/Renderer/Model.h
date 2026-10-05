#pragma once
#include <string>
#include <vector>

#include "Mesh.h"

// Assimp types appear only as pointers here, so forward declarations are enough
// and no translation unit that includes this header has to see the library
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
class FShader;

// A model file turned into geometry we own: its node tree is walked once at
// construction and every aiMesh found becomes one of our Meshes.
//
// Nothing here declares a destructor or a copy, which leaves the compiler free
// to generate the move operations. A vector of models depends on that: FMesh
// cannot be copied, so relocating the vector has to move.
class FModel
{
public:
    explicit FModel(const std::string& InPath);

    void Draw(const FShader& InShader) const;

private:
    void LoadModel(const std::string& InPath);
    void ProcessNode(const aiNode* InNode, const aiScene* InScene);
    FMesh ProcessMesh(const aiMesh* InMesh, const aiScene* InScene);
    std::vector<FTexture> LoadMaterialTextures(const aiMaterial* InMaterial, ETextureType InType);

    std::vector<FMesh> Meshes;
    std::vector<FTexture> LoadedTextures; // Keyed by path, so a shared file is uploaded once
    std::string Directory;                // Folder of the model file, texture names are relative to it
};
