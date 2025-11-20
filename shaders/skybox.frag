#version 420 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox; // Declare a cubemap sampler

void main()
{
    // Sample the cubemap using the 3D texture coordinates
    FragColor = texture(skybox, TexCoords);
}
