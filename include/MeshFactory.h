#pragma once
#include "Mesh.h"

inline Mesh createCube(float size = 1.0f) {
    Mesh cube;

    float s = size / 2.0f;

    std::vector<GLfloat> positions = {
        -s,-s,-s,1,  s,-s,-s,1,  s, s,-s,1, -s, s,-s,1,
        -s,-s, s,1,  s,-s, s,1,  s, s, s,1, -s, s, s,1
    };

    std::vector<GLfloat> colors = {
        1,0,0,1, 0,1,0,1, 0,0,1,1, 1,1,0,1,
        1,0,1,1, 0,1,1,1, 1,1,1,1, 0,0,0,1
    };

    std::vector<GLuint> indices = {
        0,1,2, 2,3,0, // backk
        4,5,6, 6,7,4, // front
        0,4,7, 7,3,0, // left
        1,5,6, 6,2,1, // right
        3,2,6, 6,7,3, // top
        0,1,5, 5,4,0  // bottom
    };

    cube.setup(positions, colors, indices);
    return cube;
}

