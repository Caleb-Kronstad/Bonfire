#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"
#include "ParamDatabase.hpp"
#include "Ray.hpp"

namespace Bonfire
{
    class Model
    {
    public:
        Model(std::string path);
        
        void Load();
        void Draw(Shader& shader, std::shared_ptr<Material> material);
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

        AABB CalculateAABB() const;

    public:
        uint32_t param_id;
        std::string name;
        std::string path;
        std::string directory;
        std::vector<Mesh> meshes;
        std::vector<std::pair<std::string, TEXTURE_TYPE>> extracted_texture_paths;
    };
}
