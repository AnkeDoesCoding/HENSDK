#version 460 core
out vec4 FragColour;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoord;

#define MAX_POINT_LIGHTS 100
#define MAX_SPOT_LIGHTS 100

struct Material 
{
    sampler2D Diffuse;
    sampler2D Specular;    

    vec3 Colour;
    float Shininess;
}; 

// vec4 Colour (x, y, z, w {intensity})
// vec4 Attenuation (x {constant}, y {linear}, z {quadratic}, w {unused})
// vec4 Angles (x {innercuttoff}, y {outercuttoff}, z {unused}, w {unused})

struct DirLight 
{
    vec4 Colour;

    vec3 Ambient;
    float Pad0;
    vec3 Direction;
    float Pad1;
};  

struct SpotLight 
{
    vec4 Colour;

    vec3 Ambient;
    float Pad0;
    vec3 Position;
    float Pad1;
    vec3 Direction;
    float Pad2;

    vec4 Angles;

    vec4 Attenuation;
};

struct PointLight
{
    vec4 Colour;

    vec3 Ambient;
    float Pad0;
    vec3 Position; 
    float Pad1;

    vec4 Attenuation;
};
  
uniform vec3 uViewPos;

uniform Material uMaterial;

layout(std140, binding = 1) uniform uLights
{
    PointLight uPointLights[MAX_SPOT_LIGHTS];
    SpotLight uSpotLights[MAX_SPOT_LIGHTS];

    DirLight uDirLight;

    int uNumberOfPointLights;
    int uNumberOfSpotLights;

    int uHasDirectionalLight;
};

vec3 CalculateLighting(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.Direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uMaterial.Shininess);

    vec3 ambient = light.Ambient * vec3(texture(uMaterial.Diffuse, TexCoord));
    vec3 diffuse = light.Colour.xyz  * diff * vec3(texture(uMaterial.Diffuse, TexCoord)) * light.Colour.w * uMaterial.Colour;
    vec3 specular = light.Colour.xyz * spec * vec3(texture(uMaterial.Specular, TexCoord)) * light.Colour.w * uMaterial.Colour;

    return (ambient + diffuse + specular);
}

vec3 CalculateLighting(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.Position - FragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uMaterial.Shininess);

    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (light.Attenuation.x + light.Attenuation.y * distance + light.Attenuation.z * (distance * distance));

    vec3 ambient = light.Ambient * vec3(texture(uMaterial.Diffuse, TexCoord));
    vec3 diffuse = light.Colour.xyz * diff * vec3(texture(uMaterial.Diffuse, TexCoord)) * light.Colour.w * uMaterial.Colour;
    vec3 specular = light.Colour.xyz * spec * vec3(texture(uMaterial.Specular, TexCoord)) * light.Colour.w * uMaterial.Colour;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
} 

vec3 CalculateLighting(SpotLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.Position - FragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uMaterial.Shininess);

    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (light.Attenuation.x + light.Attenuation.y * distance + light.Attenuation.z * (distance * distance));

    float theta = dot(lightDir, normalize(-light.Direction));
    float epsilon = light.Angles.x - light.Angles.y;
    float intensity = clamp((theta - light.Angles.y) / epsilon, 0.0, 1.0);

    vec3 ambient  = light.Ambient * vec3(texture(uMaterial.Diffuse, TexCoord));
    vec3 diffuse  = light.Colour.xyz * diff * vec3(texture(uMaterial.Diffuse, TexCoord)) * light.Colour.w * uMaterial.Colour;
    vec3 specular = light.Colour.xyz * spec * vec3(texture(uMaterial.Specular, TexCoord)) * light.Colour.w * uMaterial.Colour;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);

    vec3 result;

    if (uHasDirectionalLight == 1)
    {
        result = CalculateLighting(uDirLight, normal, viewDir);
    }

    for (int i = 0; i < min(uNumberOfPointLights, MAX_POINT_LIGHTS); i++)
    {
        result += CalculateLighting(uPointLights[i], normal, viewDir);
    }

    for (int i = 0; i < min(uNumberOfSpotLights, MAX_SPOT_LIGHTS); i++)
    {
        result += CalculateLighting(uSpotLights[i], normal, viewDir);
    }

    FragColour =  vec4(result, 1.0);
} 

