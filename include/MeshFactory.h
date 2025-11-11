#pragma once
#include "Mesh.h"

inline Mesh createCube(float size = 1.0f) {
    Mesh cube;
    float s = size / 2.0f;

    std::vector<GLfloat> positions = {
        // back face (red)
        -s,-s,-s,1,  s,-s,-s,1,  s, s,-s,1, -s, s,-s,1,
        // front face (green)
        -s,-s, s,1,  s,-s, s,1,  s, s, s,1, -s, s, s,1,
        // left face (blue)
        -s,-s,-s,1, -s, s,-s,1, -s, s, s,1, -s,-s, s,1,
        // right face (yellow)
         s,-s,-s,1,  s, s,-s,1,  s, s, s,1,  s,-s, s,1,
        // top face (magenta)
        -s, s,-s,1,  s, s,-s,1,  s, s, s,1, -s, s, s,1,
        // bottom face (cyan)
        -s,-s,-s,1,  s,-s,-s,1,  s,-s, s,1, -s,-s, s,1
    };

    std::vector<GLfloat> colors = {
        // back - red
        1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1,
        // front - green
        0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1,
        // left - blue
        0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1,
        // right - yellow
        1,1,0,1, 1,1,0,1, 1,1,0,1, 1,1,0,1,
        // top - magenta
        1,0,1,1, 1,0,1,1, 1,0,1,1, 1,0,1,1,
        // bottom - cyan
        0,1,1,1, 0,1,1,1, 0,1,1,1, 0,1,1,1
    };

    std::vector<GLuint> indices = {
        0,1,2, 2,3,0,        // back
        4,5,6, 6,7,4,        // front
        8,9,10, 10,11,8,     // left
        12,13,14, 14,15,12,  // right
        16,17,18, 18,19,16,  // top
        20,21,22, 22,23,20   // bottom
    };

    cube.setup(positions, colors, indices);
    return cube;
}

