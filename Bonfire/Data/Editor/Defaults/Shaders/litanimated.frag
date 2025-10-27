#version 460 core
out vec4 FragColor;

in VERT_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} frag_in;

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

// shadows
uniform sampler2D shadow_map;
uniform samplerCube point_shadow_map;
uniform float far_plane;

uniform int num_point_lights;
uniform int num_spot_lights;

uniform bool is_emissive;

uniform DirectionalLight directional_light;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
float ShadowCalculation(vec4 frag_pos_light_space, vec3 normal, vec3 light_direction);
float PointShadowCalculation(vec3 frag_pos, vec3 lightPos);

float gamma = 1.0;
vec3 sameple_offset_directions[20] = vec3[]
(
vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

void main()
{
    // properties
    vec3 norm = normalize(frag_in.Normal);
    vec3 view_dir = normalize(view_pos - frag_in.FragPos);

    // direction
    vec3 result;
    result += CalculateDirectionalLight(directional_light, norm, view_dir);

    // point
    for (int i = 0; i < num_point_lights; i++){
        result += CalculatePointLight(point_lights[i], norm, frag_in.FragPos, view_dir);
    }

    // spot
    for (int i = 0; i < num_spot_lights; i++){
        result += CalculateSpotLight(spot_lights[i], norm, frag_in.FragPos, view_dir);
    }

    vec3 emission = texture(material.emission, frag_in.TexCoords).rgb;
    if (is_emissive == true)
    {
        result += emission;
    }
    vec4 fragColor = vec4(result, 1.0);
    FragColor = fragColor;

    FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}

float ShadowCalculation(vec4 frag_pos_light_space, vec3 normal, vec3 light_direction)
{
    // perspective divide
    vec3 projCoords = frag_pos_light_space.xyz / frag_pos_light_space.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closest_depth = texture(shadow_map, projCoords.xy).r;

    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if (projCoords.z > 1.0)
    shadow = 0.0;

    return shadow;
}

float PointShadowCalculation(vec3 frag_pos, vec3 lightPos)
{
    vec3 frag_to_light = frag_pos - lightPos;
    float closest_depth = texture(point_shadow_map, frag_to_light).r;
    closest_depth *= far_plane;
    float currentDepth = length(frag_to_light);

    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float view_distance = length(view_pos - frag_pos);
    float diskRadius = (1.0 + (view_distance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closest_depth = texture(point_shadow_map, frag_to_light + sameple_offset_directions[i] * diskRadius).r;
        closest_depth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closest_depth)
        shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
    // directional light calculations
    vec3 light_direction = normalize(light.direction - frag_in.FragPos);
    //vec3 light_direction = normalize(-light.direction);
    vec3 color = texture(material.diffuse, frag_in.TexCoords).rgb;

    // diffuse
    float diff = max(dot(light_direction, normal), 0.0);

    // specular
    // phong
    //	vec3 reflectDir = reflect(-light_direction, normal);
    //	float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    // blinn-phong
    vec3 halfwayDir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * pow(color, vec3(gamma));
    vec3 specular = light.specular * spec * color;

    float shadow = ShadowCalculation(frag_in.FragPosLightSpace, normal, light_direction);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    // point light calculations
    vec3 light_direction = normalize(light.position - frag_pos);
    vec3 color = texture(material.diffuse, frag_in.TexCoords).rgb;

    // diffuse
    float diff = max(dot(normal, light_direction), 0.0);

    // specular
    // phong
    //	vec3 reflectDir = reflect(-light_direction, normal);
    //	float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    // blinn-phong
    vec3 halfwayDir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float dist = length(light.position - frag_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * pow(color, vec3(gamma));
    vec3 specular = light.specular * spec * color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = PointShadowCalculation(frag_in.FragPos, light.position);

    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    // spot light calculations
    vec3 light_direction = normalize(light.position - frag_in.FragPos);
    vec3 color = texture(material.diffuse, frag_in.TexCoords).rgb;

    // diffuse
    float diff = max(dot(normal, light_direction), 0.0);

    // specular
    // phong
    //	vec3 reflectDir = reflect(-light_direction, normal);
    //	float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    // blinn-phong
    vec3 halfwayDir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // soft edges
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = (light.cut_off - light.outer_cut_off);
    float intensity = smoothstep(0.0, 1.0, (theta - light.outer_cut_off) / epsilon);

    // attenuation
    float dist = length(light.position - frag_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * pow(texture(material.diffuse, frag_in.TexCoords).rgb, vec3(gamma));
    vec3 specular = light.specular * spec * color;

    diffuse *= intensity;
    specular *= intensity;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}