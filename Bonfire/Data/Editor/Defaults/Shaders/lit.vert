#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4 aBoneWeights;

out VERT_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} VertOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_matrix;
uniform bool reverse_normals;

const int MAX_BONES = 128;
uniform mat4 bone_transforms[MAX_BONES];
uniform bool is_animated;

void main()
{
    vec4 skinned_pos = vec4(aPos, 1.0);
    vec3 skinned_normal = aNormal;

    if (is_animated)
    {
        mat4 bone_transform = mat4(0.0);

        if (aBoneIds.x >= 0) bone_transform += bone_transforms[aBoneIds.x] * aBoneWeights.x;
        if (aBoneIds.y >= 0) bone_transform += bone_transforms[aBoneIds.y] * aBoneWeights.y;
        if (aBoneIds.z >= 0) bone_transform += bone_transforms[aBoneIds.z] * aBoneWeights.z;
        if (aBoneIds.w >= 0) bone_transform += bone_transforms[aBoneIds.w] * aBoneWeights.w;

        float total_weight = aBoneWeights.x + aBoneWeights.y + aBoneWeights.z + aBoneWeights.w;
        if (total_weight > 0.0) {
            skinned_pos = bone_transform * vec4(aPos, 1.0);
            skinned_normal = mat3(bone_transform) * aNormal;
        }
    }

    vec4 world_pos = model * skinned_pos;
    VertOut.FragPos = world_pos.xyz;

    mat3 normal_matrix = transpose(inverse(mat3(model)));
    VertOut.Normal = normalize(normal_matrix * skinned_normal);
    if (reverse_normals)
    VertOut.Normal = -VertOut.Normal;

    VertOut.TexCoords = aTexCoords;
    VertOut.FragPosLightSpace = light_space_matrix * world_pos;
    gl_Position = projection * view * world_pos;
}