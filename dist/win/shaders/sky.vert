#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // We still need this for the sphere mesh, even if not used for normal mapping
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // We'll use this to scale the sphere very large

void main()
{
    TexCoords = aTexCoords; // Pass through original texture coordinates for sphere
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
