#pragma once
#include "Vertex.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Mesh {
public:
  GLuint vao = 0;
  GLuint vbo = 0;
  GLuint ebo = 0;
  GLuint indexCount = 0;

  Mesh() = default;

  void setup(const std::vector<Vertex> &vertices,
             const std::vector<GLuint> &indices);

  void draw() const;
};
