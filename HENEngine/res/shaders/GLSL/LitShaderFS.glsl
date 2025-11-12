#version 460 core
out vec4 FragColor;

struct Material 
{
    sampler2D Diffuse;
    sampler2D Specular;    
    float Shininess;
}; 

struct Light 
{
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 uViewPos;
uniform Material uMaterial;
uniform Light uLight;

void main()
{
    // ambient
    vec3 ambient = uLight.Ambient * vec3(texture(uMaterial.Diffuse, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLight.Position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uLight.Diffuse * diff * vec3(texture(uMaterial.Diffuse, TexCoords));  
    
    // specular
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.Shininess);
    vec3 specular = uLight.Specular * spec * vec3(texture(uMaterial.Specular, TexCoords));
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 