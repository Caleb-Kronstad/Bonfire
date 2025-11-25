#version 460 core
out vec4 FragColor;

in VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
} frag_in;

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
    FragColor = texture(material.diffuse, frag_in.TexCoords);
}