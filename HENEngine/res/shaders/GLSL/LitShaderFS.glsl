#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 ViewPos;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform sampler2D Texture;

float Specular = 0.5;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);

    vec3 diffuse = max(dot(norm, lightDir), 0.0) * LightColor;

    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 spec = pow(max(dot(viewDir, reflectDir), 0.0), 128) * Specular * LightColor;

    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * LightColor;

    FragColor = texture(Texture, TexCoord) * vec4((ambient + diffuse + spec), 1.0);
}