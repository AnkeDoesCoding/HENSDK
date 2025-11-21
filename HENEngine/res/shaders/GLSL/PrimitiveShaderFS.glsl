#version 460 core
out vec4 FragColour;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 uColour;

void main()
{
    vec3 lightColour = vec3(1.0, 1.0, 1.0);
    vec3 lightDir = vec3(-1.0, 0.6, 0.8);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColour;

    float diff = max(dot(normalize(Normal), normalize(lightDir)), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 result = (ambient + diffuse) * uColour;
    FragColour = vec4(result, 1.0);
}