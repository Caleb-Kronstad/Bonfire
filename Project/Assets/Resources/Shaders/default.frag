#version 460 core
out vec4 FragColor;

in VERT_OUT {
    vec2 TexCoords;
} frag_in;

struct Material {
    sampler2D diffuse;
};

uniform Material material;

void main()
{
    FragColor = texture(material.diffuse, frag_in.TexCoords);
}