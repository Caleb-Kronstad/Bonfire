#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"

namespace Bonfire
{
    class Model
    {
    public:
        Model(std::string path);
        
        void Load();
        void Draw(Shader& shader, const std::vector<std::shared_ptr<Texture>>& textures);
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

    public:
        std::string path;
        std::string directory;
        std::vector<Mesh> meshes;
        
    };
}
