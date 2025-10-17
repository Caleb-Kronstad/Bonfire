#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VERT_OUT {
    vec2 TexCoords;
} vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vert_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}