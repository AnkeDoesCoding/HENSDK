#version 460 core
out vec4 FragColour;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 uLightDir;
uniform vec3 uLightColour;
uniform float uLightIntensity;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 light = normalize(uLightDir);
    
    float diff = max(dot(norm, light), 0.0);
    vec3 diffuse = diff * uLightColour * uLightIntensity;
    
    vec3 ambient = vec3(0.25);

    vec4 texColour = vec4(1.0f);

    vec3 finalColour = texColour.rgb * (diffuse + ambient);

    FragColour = vec4(finalColour, 1.0);
}
