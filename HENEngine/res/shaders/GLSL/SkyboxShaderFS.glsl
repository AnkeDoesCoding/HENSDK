#version 460
out vec4 FragColour;

in vec3 TexCoord;

uniform samplerCube uSkyboxCubemap;

void main()
{    

    FragColour = texture(uSkyboxCubemap, TexCoord);
}