#version 420 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D skyboxTexture; // Our equirectangular map

void main()
{
    // The sphere mesh's texture coordinates should already be suitable for the equirectangular map.
    // However, if the sphere UVs are not quite right, or you want to map based on world position
    // (which is more common for skybox where rotation affects the view of the skybox),
    // you might need a conversion. But for a simple large sphere, the mesh UVs should be fine.
    // If you see distortion, you might need to adjust UVs or the sphere generation itself.
    FragColor = texture(skyboxTexture, TexCoords);
}
