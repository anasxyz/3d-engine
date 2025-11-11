#pragma once
#include "Mesh.h"

inline Mesh createCube(float size = 1.0f) {
  Mesh cube;
  float s = size / 2.0f;

  std::vector<GLfloat> positions = {
      // back face (red)
      -s, -s, -s, 1, s, -s, -s, 1, s, s, -s, 1, -s, s, -s, 1,
      // front face (green)
      -s, -s, s, 1, s, -s, s, 1, s, s, s, 1, -s, s, s, 1,
      // left face (blue)
      -s, -s, -s, 1, -s, s, -s, 1, -s, s, s, 1, -s, -s, s, 1,
      // right face (yellow)
      s, -s, -s, 1, s, s, -s, 1, s, s, s, 1, s, -s, s, 1,
      // top face (magenta)
      -s, s, -s, 1, s, s, -s, 1, s, s, s, 1, -s, s, s, 1,
      // bottom face (cyan)
      -s, -s, -s, 1, s, -s, -s, 1, s, -s, s, 1, -s, -s, s, 1};

  std::vector<GLfloat> colors = {// back - red
                                 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1,
                                 // front - green
                                 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                                 // left - blue
                                 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
                                 // right - yellow
                                 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1,
                                 // top - magenta
                                 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
                                 // bottom - cyan
                                 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
                                 1};

  std::vector<GLfloat> normals;

  // back face (0, 0, -1)
  for (int i = 0; i < 4; ++i)
    normals.insert(normals.end(), {0, 0, -1});
  // front face (0, 0, 1)
  for (int i = 0; i < 4; ++i)
    normals.insert(normals.end(), {0, 0, 1});
  // left face (-1, 0, 0)
  for (int i = 0; i < 4; ++i)
    normals.insert(normals.end(), {-1, 0, 0});
  // right face (1, 0, 0)
  for (int i = 0; i < 4; ++i)
    normals.insert(normals.end(), {1, 0, 0});
  // top face (0, 1, 0)
  for (int i = 0; i < 4; ++i)
    normals.insert(normals.end(), {0, 1, 0});
  // bottom face (0, -1, 0)
  for (int i = 0; i < 4; ++i)
    normals.insert(normals.end(), {0, -1, 0});

  std::vector<GLuint> indices = {
      0,  1,  2,  2,  3,  0,  // back
      4,  5,  6,  6,  7,  4,  // front
      8,  9,  10, 10, 11, 8,  // left
      12, 13, 14, 14, 15, 12, // right
      16, 17, 18, 18, 19, 16, // top
      20, 21, 22, 22, 23, 20  // bottom
  };

  cube.setup(positions, colors, normals, indices);
  return cube;
}

inline Mesh createSphere(float radius = 1.0f, int stacks = 16,
                         int slices = 32) {
  Mesh sphere;

  if (stacks < 2)
    stacks = 2;
  if (slices < 3)
    slices = 3;

  std::vector<GLfloat> positions;
  std::vector<GLfloat> colors;
  std::vector<GLfloat> normals;
  std::vector<GLuint> indices;

  for (int i = 0; i <= stacks; ++i) {
    float v = (float)i / (float)stacks;
    float phi = glm::pi<float>() * v;

    float sinPhi = sin(phi);
    float cosPhi = cos(phi);

    for (int j = 0; j < slices; ++j) {
      float u = (float)j / (float)slices;
      float theta = glm::two_pi<float>() * u;

      float x = radius * sinPhi * cos(theta);
      float y = radius * cosPhi;
      float z = radius * sinPhi * sin(theta);

      // position
      positions.push_back(x);
      positions.push_back(y);
      positions.push_back(z);
      positions.push_back(1.0f);

      // normal
      glm::vec3 n = glm::normalize(glm::vec3(x, y, z));
      normals.push_back(n.x);
      normals.push_back(n.y);
      normals.push_back(n.z);

      // color
      // derived from normal which should be continous across the seam now (i
      // think)
      glm::vec3 color = (n * 0.5f) + glm::vec3(0.5f);
      colors.push_back(color.r);
      colors.push_back(color.g);
      colors.push_back(color.b);
      colors.push_back(1.0f);
    }
  }

  // incides
  // use modulo to wrap around the longitude seam properly
  for (int i = 0; i < stacks; ++i) {
    for (int j = 0; j < slices; ++j) {
      int nextJ = (j + 1) % slices;

      GLuint first = (GLuint)(i * slices + j);
      GLuint second = (GLuint)((i + 1) * slices + j);
      GLuint firstNext = (GLuint)(i * slices + nextJ);
      GLuint secondNext = (GLuint)((i + 1) * slices + nextJ);

      // triangle 1
      indices.push_back(first);
      indices.push_back(second);
      indices.push_back(firstNext);

      // triangle 2
      indices.push_back(second);
      indices.push_back(secondNext);
      indices.push_back(firstNext);
    }
  }

  sphere.setup(positions, colors, normals, indices);

  return sphere;
}
