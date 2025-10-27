#include "bonfire_pch.hpp"
#include "SkeletalModel.hpp"

namespace Bonfire
{
    SkeletalModel::SkeletalModel(const std::string& path)
        : Model(path), ai_scene(nullptr) {
        LoadSkeletalModel(path);
    }
  
    SkeletalModel::~SkeletalModel()
    {
        for (auto& mesh : skeletal_meshes)
        {
            if (mesh.vertex_array != 0) glDeleteVertexArrays(1, &mesh.vertex_array);
            if (mesh.vertex_buffer != 0) glDeleteBuffers(1, &mesh.vertex_buffer);
            if (mesh.element_buffer != 0) glDeleteBuffers(1, &mesh.element_buffer);
        }
    }
    
    void SkeletalModel::Draw(Shader& shader,  std::vector<std::shared_ptr<Material>>& materials)
    {
        // Draw all skeletal meshes
        for (size_t i = 0; i < skeletal_meshes.size(); i++)
        {
            const auto& mesh = skeletal_meshes[i];
            size_t mat_idx = (mesh.material_index < materials.size()) ? mesh.material_index : 0;
            if (materials.empty()) continue;

            const auto& material = materials[mat_idx];
            const auto& textures = material->textures;
            
            for (const std::shared_ptr<Texture>& texture : textures)
            {
                unsigned int texture_unit = 0;
                std::string texture_type_name = "diffuse";
                switch (texture->type)
                {
                case TextureType::DIFFUSE:
                    texture_unit = 0;
                    texture_type_name = "diffuse";
                    break;
                case TextureType::SPECULAR:
                    texture_unit = 1;
                    texture_type_name = "specular";
                    break;
                case TextureType::NORMAL:
                    texture_unit = 2;
                    texture_type_name = "normal";
                    break;
                case TextureType::HEIGHT:
                    texture_unit = 3;
                    texture_type_name = "height";
                    break;
                case TextureType::EMISSION:
                    texture_unit = 4;
                    texture_type_name = "emission";
                    break;
                }
			
                glActiveTexture(GL_TEXTURE0+texture_unit);
                shader.SetInt("material."+texture_type_name, static_cast<int>(texture_unit));
                glBindTexture(GL_TEXTURE_2D, texture->gl_id);
            }
        
            // bind and draw mesh
            glBindVertexArray(mesh.vertex_array);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);

            // unbind
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        }
    }

    AABB SkeletalModel::CalculateAABB() const
    {
        AABB aabb;
        for (const auto& mesh : skeletal_meshes)
        {
            for (const auto& vertex : mesh.vertices)
                aabb.Expand(vertex.position);
        }
        return aabb;
    }
  
    void SkeletalModel::LoadSkeletalModel(const std::string& path)
    {
        skeleton = std::make_shared<Skeleton>();

        std::filesystem::path abs_path = std::filesystem::absolute(path);
        std::string abs_path_str = abs_path.string();

        Log::Info("Loading skeletal model from: " + abs_path_str);
        
        Assimp::Importer importer;
        ai_scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_GenNormals |
            aiProcess_LimitBoneWeights);
  
        if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
        {
            Log::Error("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
            return;
        }
  
        this->path = path;
        this->directory = path.substr(0, path.find_last_of('/'));
  
        ExtractBoneHierarchy(ai_scene->mRootNode, -1);
  
        ProcessSkeletalNode(ai_scene->mRootNode, ai_scene);
  
        LoadAnimations(ai_scene);
  
        CalculateAABB(); // for editor ray
    }
  
    void SkeletalModel::ExtractBoneHierarchy(aiNode* node, int parent_index)
    {
        if (!node) return;
  
        std::string node_name = node->mName.C_Str();
        int current_index = -1;
        
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ExtractBoneHierarchy(node->mChildren[i], current_index);
        }
    }
  
    void SkeletalModel::ProcessSkeletalNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessSkeletalMesh(mesh, scene);
        }
  
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessSkeletalNode(node->mChildren[i], scene);
        }
    }
  
    void SkeletalModel::ProcessSkeletalMesh(aiMesh* mesh, const aiScene* scene)
    {
        SkeletalMesh skeletal_mesh;
  
        skeletal_mesh.vertices.reserve(mesh->mNumVertices);
        skeletal_mesh.indices.reserve(mesh->mNumFaces * 3);
  
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            SkeletalVertex vertex;
  
            // Position
            vertex.position = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );
  
            // Normal
            if (mesh->HasNormals())
            {
                vertex.normal = glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                );
            }
  
            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex.tex_coords = glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );
            }
            else
            {
                vertex.tex_coords = glm::vec2(0.0f, 0.0f);
            }
  
            // Tangent
            if (mesh->HasTangentsAndBitangents())
            {
                vertex.tangent = glm::vec3(
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z
                );
  
                // Bitangent
                vertex.bitangent = glm::vec3(
                    mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z
                );
            }
  
            skeletal_mesh.vertices.push_back(vertex);
        }
  
        // Load indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                {
                skeletal_mesh.indices.push_back(face.mIndices[j]);
            }
        }
  
        // Load bone weights
        if (mesh->HasBones())
            {
            LoadBones(mesh, skeletal_mesh.vertices);
        }
  
        if (mesh->mMaterialIndex >= 0)
            {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiString mat_name;
            material->Get(AI_MATKEY_NAME, mat_name);
            skeletal_mesh.material_name = mat_name.C_Str();
        }
  
        SetupMesh(skeletal_mesh);
  
        skeletal_meshes.push_back(skeletal_mesh);
    }
  
    void SkeletalModel::LoadBones(aiMesh* mesh, std::vector<SkeletalVertex>& vertices)
    {
        std::vector<std::vector<std::pair<int, float>>> vertex_bone_data(vertices.size());

       for (unsigned int i = 0; i < mesh->mNumBones; i++)
       {
           aiBone* bone = mesh->mBones[i];
           std::string bone_name = bone->mName.C_Str();
           int bone_index = -1;

           if (bone_map.find(bone_name) == bone_map.end())
           {
               aiMatrix4x4 offset = bone->mOffsetMatrix;
               glm::mat4 offset_matrix;

               offset_matrix[0][0] = offset.a1; offset_matrix[1][0] = offset.a2;
               offset_matrix[2][0] = offset.a3; offset_matrix[3][0] = offset.a4;
               offset_matrix[0][1] = offset.b1; offset_matrix[1][1] = offset.b2;
               offset_matrix[2][1] = offset.b3; offset_matrix[3][1] = offset.b4;
               offset_matrix[0][2] = offset.c1; offset_matrix[1][2] = offset.c2;
               offset_matrix[2][2] = offset.c3; offset_matrix[3][2] = offset.c4;
               offset_matrix[0][3] = offset.d1; offset_matrix[1][3] = offset.d2;
               offset_matrix[2][3] = offset.d3; offset_matrix[3][3] = offset.d4;

               int parent_index = -1;
               aiNode* bone_node = ai_scene->mRootNode->FindNode(bone->mName);
               if (bone_node && bone_node->mParent)
               {
                   std::string parent_name = bone_node->mParent->mName.C_Str();
                   if (bone_map.find(parent_name) != bone_map.end())
                   {
                       parent_index = bone_map[parent_name];
                   }
               }

               skeleton->AddBone(bone_name, parent_index, offset_matrix);
               bone_index = skeleton->GetBoneIndex(bone_name);
               bone_map[bone_name] = bone_index;
           }
           else
           {
               bone_index = bone_map[bone_name];
           }

           for (unsigned int j = 0; j < bone->mNumWeights; j++)
           {
               unsigned int vertex_id = bone->mWeights[j].mVertexId;
               float weight = bone->mWeights[j].mWeight;

               if (vertex_id < vertices.size())
               {
                   vertex_bone_data[vertex_id].push_back({bone_index, weight});
               }
           }
       }

        int vertices_with_more_than_4 = 0;
        int max_influences = 0;
  
        for (size_t i = 0; i < vertices.size(); i++)
        {
            auto& bone_list = vertex_bone_data[i];
  
            if (bone_list.size() > 4)
            {
                vertices_with_more_than_4++;
                max_influences = (std::max)(max_influences, (int)bone_list.size());
  
                std::sort(bone_list.begin(), bone_list.end(),
                    [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
                        return a.second > b.second;
                    });
  
                bone_list.resize(4);
            }
  
            for (size_t j = 0; j < bone_list.size() && j < 4; j++)
            {
                vertices[i].bone_ids[j] = bone_list[j].first;
                vertices[i].bone_weights[j] = bone_list[j].second;
            }
        }
  
        if (vertices_with_more_than_4 > 0)
        {
            Log::Warning("Mesh has " + std::to_string(vertices_with_more_than_4) + " vertices with >4 bone influences (max: " + std::to_string(max_influences) + "). Keeping top 4 weights per vertex.");
        }
  
        for (auto& vertex : vertices)
        {
            float total_weight = vertex.bone_weights.x + vertex.bone_weights.y + vertex.bone_weights.z + vertex.bone_weights.w;
            if (total_weight > 0.0f)
            {
                vertex.bone_weights /= total_weight;
            }
        }
    }
  
    void SkeletalModel::LoadAnimations(const aiScene* scene)
    {
        for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
            aiAnimation* anim = scene->mAnimations[i];
  
            std::string anim_name = anim->mName.C_Str();
            if (anim_name.empty()) {
                anim_name = "Animation_" + std::to_string(i);
                Log::Error("ANIM NAME EMPTY");
            }
  
            float duration = static_cast<float>(anim->mDuration);
            float ticks_per_second = static_cast<float>(anim->mTicksPerSecond);
            if (ticks_per_second == 0.0f) {
                ticks_per_second = 25.0f;
            }
  
            auto animation = std::make_shared<Animation>(anim_name, duration, ticks_per_second);
  
            for (unsigned int j = 0; j < anim->mNumChannels; j++) {
                aiNodeAnim* track = anim->mChannels[j];
                std::string track_name = track->mNodeName.C_Str();
  
                auto anim_track = std::make_shared<AnimationTrack>(track_name);
  
                for (unsigned int k = 0; k < track->mNumPositionKeys; k++) {
                    aiVectorKey key = track->mPositionKeys[k];
                    anim_track->AddPositionKeyframe(
                        static_cast<float>(key.mTime),
                        glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                    );
                }
  
                for (unsigned int k = 0; k < track->mNumRotationKeys; k++) {
                    aiQuatKey key = track->mRotationKeys[k];
                    anim_track->AddRotationKeyframe(
                        static_cast<float>(key.mTime),
                        glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)
                    );
                }
                
                for (unsigned int k = 0; k < track->mNumScalingKeys; k++) {
                    aiVectorKey key = track->mScalingKeys[k];
                    anim_track->AddScaleKeyframe(
                        static_cast<float>(key.mTime),
                        glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                    );
                }
  
                animation->AddTrack(anim_track);
            }
  
            animations.push_back(animation);
        }
    }
  
    void SkeletalModel::SetupMesh(SkeletalMesh& mesh)
    {
        // Generate buffers
        glGenVertexArrays(1, &mesh.vertex_array);
        glGenBuffers(1, &mesh.vertex_buffer);
        glGenBuffers(1, &mesh.element_buffer);
  
        glBindVertexArray(mesh.vertex_array);
  
        // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER,
                     mesh.vertices.size() * sizeof(SkeletalVertex),
                     &mesh.vertices[0],
                     GL_STATIC_DRAW);
  
        // Upload index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mesh.indices.size() * sizeof(unsigned int),
                     &mesh.indices[0],
                     GL_STATIC_DRAW);
  
        // Vertex attributes
        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)0);
  
        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, normal));
  
        // Texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, tex_coords));
  
        // Tangent
        //glEnableVertexAttribArray(3);
        //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, tangent));
  
        // Bitangent
        //glEnableVertexAttribArray(4);
        //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, bitangent));
  
        // Bone IDs (ivec4)
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, bone_ids));
  
        // Bone Weights (vec4)
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, bone_weights));
  
        glBindVertexArray(0);
    }
  
    void SkeletalModel::CalculateAABB()
    {
        if (skeletal_meshes.empty()) return;
  
        glm::vec3 min_point(FLT_MAX);
        glm::vec3 max_point(-FLT_MAX);
  
        for (const auto& mesh : skeletal_meshes) {
            for (const auto& vertex : mesh.vertices) {
                min_point = (glm::min)(min_point, vertex.position);
                max_point = (glm::max)(max_point, vertex.position);
            }
        }
    }

}