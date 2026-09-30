#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class ETextureType : std::uint8_t;
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct FTexture;
class FMesh;
class FShader;

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
    std::vector<FTexture> LoadedTextures; // Path cache
    std::string Directory;
};
