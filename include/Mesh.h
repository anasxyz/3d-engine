#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>

class Mesh {
public:
  GLuint vao;
  GLuint vboPositions = 0;
  GLuint vboColours = 0;
  GLuint eboIndices = 0;
  GLuint indexCount = 0;

  Mesh() = default;

  void setup(std::vector<GLfloat> &positions, std::vector<GLfloat> &colours,
             std::vector<GLuint> &indices) {}

  void draw() {}
};
