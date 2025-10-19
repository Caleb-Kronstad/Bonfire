#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"
#include "ParamDatabase.hpp"
#include "Ray.hpp"

namespace Bonfire
{
    struct MaterialTexture
    {
        std::string path;
        TEXTURE_TYPE type;
    };
    
    class Model
    {
    public:
        Model(std::string path);
        
        void Load();
        void Draw(Shader& shader, const std::vector<std::shared_ptr<Texture>>& textures);
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

        AABB CalculateAABB() const;

    private:
        void ProcessMaterials(const aiScene* scene);

    public:
        std::vector<MaterialTexture> extracted_textures;
        uint32_t param_id;
        std::string name;
        std::string path;
        std::string directory;
        std::vector<Mesh> meshes;
    };
}
