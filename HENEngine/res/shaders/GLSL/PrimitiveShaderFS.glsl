#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 colour;

vec3 lightDir = normalize(vec3(-1.0, 0.6, 0.8));
vec3 lightColour = vec3(1.0, 1.0, 1.0);

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColour;
  	
    // diffuse 
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 result = (ambient + diffuse) * colour;
    FragColor = vec4(result, 1.0);
}