#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceModel;

out VERT_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} VertOut;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_matrix;
uniform bool reverse_normals;

uniform vec2 texture_tiling;
uniform vec2 texture_offset;

void main()
{
    vec4 world_pos = aInstanceModel * vec4(aPos, 1.0);
    VertOut.FragPos = world_pos.xyz;

    mat3 normal_matrix = transpose(inverse(mat3(aInstanceModel)));
    VertOut.Normal = normalize(normal_matrix * aNormal);
    if (reverse_normals)
    {
        VertOut.Normal = -VertOut.Normal;
    }

    VertOut.TexCoords = aTexCoords * texture_tiling + texture_offset;
    VertOut.FragPosLightSpace = light_space_matrix * world_pos;
    gl_Position = projection * view * world_pos;
}