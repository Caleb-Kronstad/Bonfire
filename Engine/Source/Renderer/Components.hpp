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

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        
        Transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
        glm::quat GetOrientation();
        glm::mat4 GetTransformMatrix();
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