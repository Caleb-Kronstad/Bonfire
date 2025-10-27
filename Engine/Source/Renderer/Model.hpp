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
        virtual void Draw(Shader& shader, std::vector<std::shared_ptr<Material>>& materials);
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

        virtual AABB CalculateAABB() const;
        virtual bool IsAnimated() const { return false; }

    public:
        bool casts_shadow = true;
        uint32_t param_id;
        std::string name;
        std::string path;
        std::string directory;
        std::vector<Mesh> meshes;
        std::vector<std::pair<std::string, TextureType>> extracted_texture_paths;
        std::vector<uint32_t> mesh_material_indices;
    };
}
