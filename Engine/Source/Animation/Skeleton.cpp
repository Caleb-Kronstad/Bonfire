#include "bonfire_pch.hpp"
#include "Skeleton.hpp"

namespace Bonfire
{
    void Skeleton::AddBone(const std::string& name, int parent_index, const glm::mat4& offset)
    {
        int index = static_cast<int>(bones.size());
        bones.emplace_back(name, index, offset);
        parent_indices.push_back(parent_index);
        bone_map.insert_or_assign(name, index);
    }

    int Skeleton::GetBoneIndex(const std::string& name) const
    {
        auto it = bone_map.find(name);
        return it != bone_map.end() ? it->second : -1;
    }
}
