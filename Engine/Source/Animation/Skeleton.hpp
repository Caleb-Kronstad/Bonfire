#pragma once

namespace Bonfire
{
    struct Bone
    {
        std::string name;
        int index;
        glm::mat4 offset_matrix;

        Bone(const std::string& name, int index, const glm::mat4& offset)
            : name(name), index(index), offset_matrix(offset) {}
    };
    
    class Skeleton
    {
    public:
        Skeleton() = default;

        void AddBone(const std::string& name, int parent_index, const glm::mat4& offset);
        int GetBoneIndex(const std::string& name) const;
        const Bone& GetBone(int bone_index) const { return bones[bone_index]; }
        int GetBoneCount() const { return static_cast<int>(bones.size()); }
        int GetParentIndex(int bone_index) const { return parent_indices[bone_index]; }
        const glm::mat4& GetOffsetMatrix(int bone_index) const { return bones[bone_index].offset_matrix; }
        const std::vector<Bone>& GetBones() { return bones; }

    private:
        std::vector<Bone> bones;
        std::vector<int> parent_indices;
        std::unordered_map<std::string, int> bone_map;
    };
}
