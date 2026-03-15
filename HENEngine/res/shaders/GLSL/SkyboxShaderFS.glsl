#version 460 core
out vec4 FragColour;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoord;

#define MAX_POINT_LIGHTS 100
#define MAX_SPOT_LIGHTS 100

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

layout(std140, binding = 1) uniform uLights
{
    PointLight uPointLights[MAX_POINT_LIGHTS];
    SpotLight uSpotLights[MAX_SPOT_LIGHTS];

    DirLight uDirLight;

    int uNumberOfPointLights;
    int uNumberOfSpotLights;

    int uHasDirectionalLight;
};

uniform vec3 uViewPos;
uniform float uSkyboxScale;

// Constants
const vec3 SKY_COLOR_LOWER = vec3(0.4, 0.6, 1.0);
const vec3 SKY_COLOR_UPPER = vec3(0.2, 0.4, 0.8);
const float HORIZON_FADE = 5000.0;

vec3 CalculateLighting(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.Direction);
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 diffuse = light.Colour.xyz * diff * light.Colour.w;
    vec3 ambient = light.Ambient;
    
    return (ambient + diffuse);
}

vec3 CalculateLighting(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 scaledLightPos = light.Position / uSkyboxScale;

    vec3 lightDir = normalize(scaledLightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    
    float distance = length(scaledLightPos - FragPos) * uSkyboxScale;
    float attenuation = 1.0 / (light.Attenuation.x + (light.Attenuation.y) * distance + light.Attenuation.z * (distance * distance));
    
    vec3 diffuse = light.Colour.xyz * diff * light.Colour.w;
    vec3 ambient = light.Ambient;
    
    return (ambient + diffuse) * attenuation;
}

vec3 CalculateLighting(SpotLight light, vec3 normal, vec3 viewDir)
{
    vec3 scaledLightPos = light.Position / uSkyboxScale;

    vec3 lightDir = normalize(scaledLightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    
    float distance = length(scaledLightPos - FragPos) * uSkyboxScale;
    float attenuation = 1.0 / (light.Attenuation.x + light.Attenuation.y * distance + light.Attenuation.z * (distance * distance));
    
    float theta = dot(lightDir, normalize(-light.Direction));
    float epsilon = light.Angles.x - light.Angles.y;
    float intensity = clamp((theta - light.Angles.y) / epsilon, 0.0, 1.0);
    
    vec3 diffuse = light.Colour.xyz * diff * light.Colour.w;
    vec3 ambient = light.Ambient;
    
    return (ambient + diffuse) * attenuation * intensity;
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    if (uHasDirectionalLight == 1)
    {
        result += CalculateLighting(uDirLight, normal, viewDir);
    }
    
    for (int i = 0; i < min(uNumberOfPointLights, MAX_POINT_LIGHTS); i++)
    {
        result += CalculateLighting(uPointLights[i], normal, viewDir);
    }
    
    for (int i = 0; i < min(uNumberOfSpotLights, MAX_SPOT_LIGHTS); i++)
    {
        result += CalculateLighting(uSpotLights[i], normal, viewDir);
    }
    
    float heightFactor = clamp(FragPos.y / HORIZON_FADE, 0.0, 1.0);
    vec3 skyColor = mix(SKY_COLOR_LOWER, SKY_COLOR_UPPER, heightFactor);
    
    float viewDist = length(FragPos);
    float fogFactor = clamp(viewDist / 50000.0, 0.0, 0.8);

    vec3 finalColor = result;
    finalColor = mix(finalColor, skyColor, 0.08);
    finalColor = mix(finalColor, skyColor, fogFactor);
    
    FragColour = vec4(finalColor, 1.0);
}