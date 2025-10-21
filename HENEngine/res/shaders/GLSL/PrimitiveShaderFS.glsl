#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 uColour;

vec3 lightDir = vec3(-1.0, 0.6, 0.8);
vec3 lightColour = vec3(1.0, 1.0, 1.0);

void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightDir);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColour;

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 result = (ambient + diffuse) * uColour;
    FragColor = vec4(result, 1.0);
}