#pragma once

#include "Core/Utility.hpp"
#include "Mesh.hpp"

namespace Bonfire
{
    class Model
    {
    public:
        Model(std::string path);

        void Load();
        void Draw(Shader& shader);

        bool AddTexture(std::shared_ptr<Texture> texture);
        bool RemoveTexture(std::shared_ptr<Texture> texture);

    private:
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

    private:
        std::string path;
        std::string directory;
        std::vector<Mesh> meshes;
        std::vector<std::shared_ptr<Texture>> textures;
    };
}
