#include "bonfire_pch.hpp"
#include "Model.hpp"

#include "Core/Utility.hpp"

namespace Bonfire
{
    Model::Model(std::string path)
        : path(path)
    {
    }
    void Model::Draw(Shader& shader, const std::vector<std::shared_ptr<Texture>>& textures)
    {
        for (Mesh mesh : meshes)
        {
            mesh.Draw(shader, textures);
        }
    }

    AABB Model::CalculateAABB() const
    {
        AABB aabb;
        for (const auto& mesh : meshes)
        {
            for (const auto& vertex : mesh.vertices)
                aabb.Expand(vertex.position);
        }
        return aabb;
    }

    void Model::ProcessMaterials(const aiScene* scene)
    {
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            aiMaterial* material = scene->mMaterials[i];

            // Extract diffuse textures
            for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++)
            {
                aiString str;
                material->GetTexture(aiTextureType_DIFFUSE, j, &str);
                std::string texture_path = str.C_Str();

                // Convert to relative path if needed
                std::filesystem::path full_path = std::filesystem::path(directory) / texture_path;
                if (std::filesystem::exists(full_path))
                {
                    std::string abs_str = full_path.string();
                    size_t data_pos = abs_str.find("Data");
                    if (data_pos != std::string::npos)
                    {
                        texture_path = abs_str.substr(data_pos);
                      std::replace(texture_path.begin(), texture_path.end(), '\\', '/');
                    }
                }

                extracted_textures.push_back({texture_path, TEXTURE_TYPE::DIFFUSE});
            }

          // Extract specular textures
            for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_SPECULAR); j++)
            {
                aiString str;
                material->GetTexture(aiTextureType_SPECULAR, j, &str);
                std::string texture_path = str.C_Str();

                std::filesystem::path full_path = std::filesystem::path(directory) / texture_path;
                if (std::filesystem::exists(full_path))
                {
                    std::string abs_str = full_path.string();
                    size_t data_pos = abs_str.find("Data");
                    if (data_pos != std::string::npos)
                    {
                        texture_path = abs_str.substr(data_pos);
                        std::replace(texture_path.begin(), texture_path.end(), '\\', '/');
                    }
                }

                extracted_textures.push_back({texture_path, TEXTURE_TYPE::SPECULAR});
            }

            // Extract normal maps
            for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_NORMALS); j++)
            {
                aiString str;
                material->GetTexture(aiTextureType_NORMALS, j, &str);
                std::string texture_path = str.C_Str();
  
                std::filesystem::path full_path = std::filesystem::path(directory) / texture_path;
                if (std::filesystem::exists(full_path))
                {
                    std::string abs_str = full_path.string();
                    size_t data_pos = abs_str.find("Data");
                    if (data_pos != std::string::npos)
                    {
                        texture_path = abs_str.substr(data_pos);
                        std::replace(texture_path.begin(), texture_path.end(), '\\', '/');
                    }
                }

                extracted_textures.push_back({texture_path, TEXTURE_TYPE::NORMAL});
            }

            // Extract height maps (sometimes used as normal maps)
            for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_HEIGHT); j++)
            {
                aiString str;
                material->GetTexture(aiTextureType_HEIGHT, j, &str);
                std::string texture_path = str.C_Str();
  
                std::filesystem::path full_path = std::filesystem::path(directory) / texture_path;
                if (std::filesystem::exists(full_path))
                {
                    std::string abs_str = full_path.string();
                    size_t data_pos = abs_str.find("Data");
                    if (data_pos != std::string::npos)
                    {
                        texture_path = abs_str.substr(data_pos);
                        std::replace(texture_path.begin(), texture_path.end(), '\\', '/');
                    }
                }

                extracted_textures.push_back({texture_path, TEXTURE_TYPE::HEIGHT});
            }
        }

        if (!extracted_textures.empty())
            Log::Info("Extracted " + std::to_string(extracted_textures.size()) + " texture(s) from model materials");
    }
    
    void Model::Load()
    {
        Assimp::Importer import;
        const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Log::Error("ERROR::ASSIMP::" + std::string(import.GetErrorString()) + "\n");
            return;
        }
        this->directory = path.substr(0, path.find_last_of('/'));

        ProcessNode(scene->mRootNode, scene);
        ProcessMaterials(scene);
    }
    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        // process node's meshes (if any)
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }
    Mesh Model::ProcessMesh(aiMesh* ai_mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        // process vertices
        for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
        {
            Vertex vertex;

            vertex.position = AssimpGLMHelpers::GetGLMVec(ai_mesh->mVertices[i]);
            vertex.normal = AssimpGLMHelpers::GetGLMVec(ai_mesh->mNormals[i]);

            if (ai_mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = ai_mesh->mTextureCoords[0][i].x;
                vec.y = ai_mesh->mTextureCoords[0][i].y;
                vertex.tex_coords = vec;
            }
            else
                vertex.tex_coords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        // process indices
        for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
        {
            aiFace face = ai_mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        return {vertices, indices};
    }
}
