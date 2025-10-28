#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
//layout (location = 3) in vec3 aTangent;
//layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4 aBoneWeights;

out VERT_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vert_out;

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
    vec4 skinnedPos = vec4(aPos, 1.0);
    vec3 skinnedNormal = aNormal;

    if (is_animated)
    {
        mat4 boneTransform = mat4(0.0);

        if (aBoneIds.x >= 0) boneTransform += bone_transforms[aBoneIds.x] * aBoneWeights.x;
        if (aBoneIds.y >= 0) boneTransform += bone_transforms[aBoneIds.y] * aBoneWeights.y;
        if (aBoneIds.z >= 0) boneTransform += bone_transforms[aBoneIds.z] * aBoneWeights.z;
        if (aBoneIds.w >= 0) boneTransform += bone_transforms[aBoneIds.w] * aBoneWeights.w;

        float totalWeight = aBoneWeights.x + aBoneWeights.y + aBoneWeights.z + aBoneWeights.w;
        if (totalWeight > 0.0) {
            skinnedPos = boneTransform * vec4(aPos, 1.0);
            skinnedNormal = mat3(boneTransform) * aNormal;
        }
    }

    vec4 worldPos = model * skinnedPos;
    vert_out.FragPos = worldPos.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vert_out.Normal = normalize(normalMatrix * skinnedNormal);
    if (reverse_normals)
    vert_out.Normal = -vert_out.Normal;

    vert_out.TexCoords = aTexCoords;
    vert_out.FragPosLightSpace = light_space_matrix * worldPos;
    gl_Position = projection * view * worldPos;
}
