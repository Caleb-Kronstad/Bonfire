#pragma once

#include "Core/Utility.hpp"

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

namespace Bonfire
{
    struct Component
    {
        bool enabled = true;
        
        virtual ~Component() = default;
    };

    struct Transform;
    struct Model;
    struct Textures;
    
    struct Transform : Component
    {
        static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::TRANSFORM;
    };

    struct Model : Component
    {
        static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::MODEL;

        std::string path;
        std::string directory;
        std::vector<Mesh> meshes;
        
        void Load(std::shared_ptr<Textures> textures);
        void Draw(Shader& shader);
        void ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Textures> textures);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Textures> textures);
        
        Model(std::string path);
    };

    struct Textures : Component
    {
        static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::TEXTURES;
        
        std::vector<std::shared_ptr<Texture>> textures;

        bool AddTexture(std::shared_ptr<Texture> texture);
        bool RemoveTexture(std::shared_ptr<Texture> texture);
        
        Textures(std::vector<std::shared_ptr<Texture>> textures = {});
    };
}
