#pragma once

#include "Renderer/Model.hpp"
#include "Animation/Skeleton.hpp"
#include "Animation/Animation.hpp"

namespace Bonfire
{
    struct SkeletalVertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 tex_coords;
      glm::vec3 tangent;
      glm::vec3 bitangent;
      glm::ivec4 bone_ids = glm::ivec4(-1); // Max 4 bones per vertex
      glm::vec4 bone_weights = glm::vec4(0.0f);

      void AddBoneData(int bone_id, float weight) {
          for (int i = 0; i < 4; i++) {
              if (bone_ids[i] < 0) {
                  bone_ids[i] = bone_id;
                  bone_weights[i] = weight;
                  return;
              }
          }
      }
    };
  
    struct SkeletalMesh {
        std::vector<SkeletalVertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int vertex_array, vertex_buffer, element_buffer;
        std::string material_name;
        uint32_t material_index = 0;
  
        SkeletalMesh() : vertex_array(0), vertex_buffer(0), element_buffer(0) {}
    };
  
    class SkeletalModel : public Model {
    public:
        SkeletalModel(const std::string& path);
        virtual ~SkeletalModel();
  
        std::shared_ptr<Skeleton> GetSkeleton() const { return skeleton; }
        const std::vector<std::shared_ptr<Animation>>& GetAnimations() const { return animations; }
        const std::vector<SkeletalMesh>& GetSkeletalMeshes() const { return skeletal_meshes; }
  
        bool IsAnimated() const override { return true; }
  
        void Draw(Shader& shader,  std::vector<std::shared_ptr<Material>>& materials) override;
        AABB CalculateAABB() const override;
  
    private:
        void LoadSkeletalModel(const std::string& path);
        void ProcessSkeletalNode(aiNode* node, const aiScene* scene);
        void ProcessSkeletalMesh(aiMesh* mesh, const aiScene* scene);
        void LoadBones(aiMesh* mesh, std::vector<SkeletalVertex>& vertices);
        void ExtractBoneHierarchy(aiNode* node, int parent_index);
        void LoadAnimations(const aiScene* scene);
        void SetupMesh(SkeletalMesh& mesh);
        void CalculateAABB();

    private:
        std::shared_ptr<Skeleton> skeleton;
        std::vector<std::shared_ptr<Animation>> animations;
        std::vector<SkeletalMesh> skeletal_meshes;
        std::unordered_map<std::string, int> bone_map;
  
        const aiScene* ai_scene;
    };
}
