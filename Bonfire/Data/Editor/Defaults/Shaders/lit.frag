#version 460 core
out vec4 FragColor;

in VERT_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} FragIn;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D height;
    sampler2D emission;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float cut_off;
    float outer_cut_off;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

uniform vec3 view_pos;
uniform Material material;

uniform sampler2D shadow_map;
uniform samplerCube point_shadow_map;
uniform float far_plane;

uniform int num_point_lights;
uniform int num_spot_lights;

uniform bool is_emissive;

uniform bool fog_enabled;
uniform vec3 fog_color;
uniform float fog_density;
uniform float fog_start;
uniform float fog_end;
uniform int fog_type;

uniform DirectionalLight directional_light;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
float ShadowCalculation(vec4 frag_pos_light_space, vec3 normal, vec3 light_direction);
float PointShadowCalculation(vec3 frag_pos, vec3 light_pos);
float CalculateFogFactor(float distance);

float gamma = 1.0;
vec3 sample_offset_directions[20] = vec3[]
(
vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

void main()
{
    vec3 norm = normalize(FragIn.Normal);
    vec3 view_dir = normalize(view_pos - FragIn.FragPos);

    vec3 result;
    result += CalculateDirectionalLight(directional_light, norm, view_dir);

    for (int i = 0; i < num_point_lights; i++){
        result += CalculatePointLight(point_lights[i], norm, FragIn.FragPos, view_dir);
    }

    for (int i = 0; i < num_spot_lights; i++){
        result += CalculateSpotLight(spot_lights[i], norm, FragIn.FragPos, view_dir);
    }

    vec3 emission = texture(material.emission, FragIn.TexCoords).rgb;
    if (is_emissive == true)
    {
        result += emission;
    }
    vec4 frag_color = vec4(result, 1.0);
    
    if (fog_enabled)
    {
        float distance = length(view_pos - FragIn.FragPos);
        float fog_factor = CalculateFogFactor(distance);
        frag_color.rgb = mix(fog_color, frag_color.rgb, fog_factor);
    }
    
    FragColor = frag_color;
    FragColor.rgb = pow(frag_color.rgb, vec3(1.0/gamma));
}

float CalculateFogFactor(float distance)
{ 
    if (fog_type == 1) // exponential fog
    {
        return clamp(exp(-fog_density * distance), 0.0, 1.0);
    }
    else if (fog_type == 2) // exponential squared fog
    {
        float exponent = fog_density * distance;
        return clamp(exp(-exponent * exponent), 0.0, 1.0);
    }
    // Linear fog
    return clamp((fog_end - distance) / (fog_end - fog_start), 0.0, 1.0);
}

float ShadowCalculation(vec4 frag_pos_light_space, vec3 normal, vec3 light_direction)
{
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;

    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texture(shadow_map, proj_coords.xy).r;

    float current_depth = proj_coords.z;

    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);

    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth  ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if (proj_coords.z > 1.0)
    shadow = 0.0;

    return shadow;
}

float PointShadowCalculation(vec3 frag_pos, vec3 light_pos)
{
    vec3 frag_to_light = frag_pos - light_pos;
    float closest_depth = texture(point_shadow_map, frag_to_light).r;
    closest_depth *= far_plane;
    float current_depth = length(frag_to_light);

    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float view_distance = length(view_pos - frag_pos);
    float disk_radius = (1.0 + (view_distance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closest_depth = texture(point_shadow_map, frag_to_light + sample_offset_directions[i] * disk_radius).r;
        closest_depth *= far_plane;
        if(current_depth - bias > closest_depth)
        shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_direction = normalize(light.direction - FragIn.FragPos);
    vec3 color = texture(material.diffuse, FragIn.TexCoords).rgb;

    float diff = max(dot(light_direction, normal), 0.0);

    vec3 halfway_dir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfway_dir), 0.0), material.shininess);

    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * pow(color, vec3(gamma));
    vec3 specular = light.specular * spec * color;

    float shadow = ShadowCalculation(FragIn.FragPosLightSpace, normal, light_direction);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 light_direction = normalize(light.position - frag_pos);
    vec3 color = texture(material.diffuse, FragIn.TexCoords).rgb;

    float diff = max(dot(normal, light_direction), 0.0);

    vec3 halfway_dir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfway_dir), 0.0), material.shininess);

    float dist = length(light.position - FragIn.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * pow(color, vec3(gamma));
    vec3 specular = light.specular * spec * color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = PointShadowCalculation(FragIn.FragPos, light.position);

    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 light_direction = normalize(light.position - FragIn.FragPos);
    vec3 color = texture(material.diffuse, FragIn.TexCoords).rgb;

    float diff = max(dot(normal, light_direction), 0.0);

    vec3 halfway_dir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfway_dir), 0.0), material.shininess);

    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = (light.cut_off - light.outer_cut_off);
    float intensity = smoothstep(0.0, 1.0, (theta - light.outer_cut_off) / epsilon);

    float dist = length(light.position - FragIn.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * pow(texture(material.diffuse, FragIn.TexCoords).rgb, vec3(gamma));
    vec3 specular = light.specular * spec * color;

    diffuse *= intensity;
    specular *= intensity;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}