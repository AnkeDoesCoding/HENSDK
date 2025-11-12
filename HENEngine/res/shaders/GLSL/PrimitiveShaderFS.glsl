#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 uColour;

vec3 LightDir = vec3(-1.0, 0.6, 0.8);
vec3 LightColour = vec3(1.0, 1.0, 1.0);

void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(LightDir);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightColour;

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * LightColour;

    vec3 result = (ambient + diffuse) * uColour;
    FragColor = vec4(result, 1.0);
}