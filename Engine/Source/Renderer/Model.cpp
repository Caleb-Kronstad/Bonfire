#include "bonfire_pch.hpp"
#include "Model.hpp"

namespace Bonfire
{
    Model::Model(std::string path)
        : path(path)
    {
    }

    void Model::Draw(Shader& shader)
    {
        for (Mesh mesh : meshes)
            mesh.Draw(shader);
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
    }

    bool Model::AddTexture(std::shared_ptr<Texture> texture)
    {
        auto it = std::find(textures.begin(), textures.end(), texture);
        if (it == textures.end())
        {
            textures.push_back(texture);
            texture->Load();
            return true;
        }
        Log::Warning("[FAILED] Texture already added to Model");
        return false;
    }
    bool Model::RemoveTexture(std::shared_ptr<Texture> texture)
    {
        auto it = std::find(textures.begin(), textures.end(), texture);
        if (it != textures.end())
        {
            textures.erase(it);
            return true;
        }
        Log::Warning("[FAILED] Model does not contain this Texture");
        return false;
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

        return {vertices, indices, textures};
    }
}
