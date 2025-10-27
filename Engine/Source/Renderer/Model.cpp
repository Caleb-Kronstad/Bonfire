#include "bonfire_pch.hpp"
#include "Model.hpp"

#include "Core/Utility.hpp"

namespace Bonfire
{
    Model::Model(std::string path)
        : path(path)
    {
    }
    void Model::Draw(Shader& shader, std::vector<std::shared_ptr<Material>>& materials)
    {
        for (size_t i = 0; i < meshes.size(); i++)
        {
            size_t mat_idx = (i < mesh_material_indices.size()) ? mesh_material_indices[i] : 0;
            mat_idx = (std::min)(mat_idx, materials.size() - 1);
            meshes[i].Draw(shader, materials[mat_idx]);
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
    
    void Model::Load()
    {
        Assimp::Importer import;
        const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Log::Error("ERROR::ASSIMP::" + std::string(import.GetErrorString()));
            return;
        }
        this->directory = path.substr(0, path.find_last_of('/'));

        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            aiMaterial* material = scene->mMaterials[i];

            auto extractTextures = [&](aiTextureType ai_type, TextureType engine_type) {
                for (unsigned int j = 0; j < material->GetTextureCount(ai_type); j++)
                {
                    aiString str;
                    material->GetTexture(ai_type, j, &str);
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
                    extracted_texture_paths.push_back({texture_path, engine_type});
                }
            };

            extractTextures(aiTextureType_DIFFUSE, TextureType::DIFFUSE);
            extractTextures(aiTextureType_SPECULAR, TextureType::SPECULAR);
            extractTextures(aiTextureType_NORMALS, TextureType::NORMAL);
            extractTextures(aiTextureType_HEIGHT, TextureType::HEIGHT);
            extractTextures(aiTextureType_EMISSIVE, TextureType::EMISSION);
        }
        ProcessNode(scene->mRootNode, scene);

        if (!extracted_texture_paths.empty())
            Log::Info("Extracted " + std::to_string(extracted_texture_paths.size()) + " textures from model");
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

        mesh_material_indices.push_back(ai_mesh->mMaterialIndex);

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
        
        return Mesh(vertices, indices);
    }

}
