#version 460 core
out vec4 FragColor;

in VS_OUT {
    in vec3 FragPos;
    in vec3 Normal;
    in vec2 TexCoords;
} fs_in;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D height;
    float shininess;
};

uniform Material material;

void main()
{
    FragColor = texture(material.diffuse, fs_in.TexCoords);
}