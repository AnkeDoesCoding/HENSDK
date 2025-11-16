#version 460 core
out vec4 FragColor;

struct Material 
{
    sampler2D Diffuse;
    sampler2D Specular;    
}; 

struct Light
{
    vec3 Colour;
    vec3 Ambient;

    float Constant;
    float Linear;
    float Quadratic;

    vec3 Position; 
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoord;
  
uniform vec3 uViewPos;
uniform Material uMaterial;
uniform Light uLight;

void main()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(uLight.Position - FragPos);

    float distance = length(uLight.Position - FragPos);
    float attentuation = 1.0 / (uLight.Constant + uLight.Linear * distance + uLight.Quadratic * (distance * distance));

    vec3 ambient = uLight.Ambient * vec3(texture(uMaterial.Diffuse, TexCoord));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLight.Colour * vec3(texture(uMaterial.Diffuse, TexCoord));

    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = spec * uLight.Colour * vec3(texture(uMaterial.Diffuse, TexCoord));  

    ambient *= attentuation;
    diffuse *= attentuation;
    specular *= attentuation;

    vec3 result = ambient + diffuse + specular;
    FragColor =  vec4(result, 1.0);
} 