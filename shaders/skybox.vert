#version 420 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // The cube's vertex positions are directly used as texture coordinates
    TexCoords = aPos;
    // Project the position but set w to 1.0 (equivalent to pos.xyww)
    // This effectively places the cube at infinite distance and makes its depth always 1.0
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
