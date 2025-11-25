#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
} vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec2 texture_tiling;
uniform vec2 texture_offset;

void main()
{
	vert_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vert_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	vert_out.TexCoords = aTexCoords * texture_tiling + texture_offset;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}