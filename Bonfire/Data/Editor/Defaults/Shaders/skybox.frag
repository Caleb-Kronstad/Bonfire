#version 460 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform bool fog_enabled;
uniform vec3 fog_color;

void main()
{    
    vec4 skybox_color = texture(skybox, TexCoords);
    if (fog_enabled)
    {
        FragColor = vec4(fog_color, 1.0);
    }
    else
    {
        FragColor = texture(skybox, TexCoords);
    }
}