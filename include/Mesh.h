#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace glm;

class Mesh {
public:
  GLuint vao;
  GLuint vboPositions = 0;
  GLuint vboColours = 0;
  GLuint vboNormals = 0;
  GLuint vboTexCoords = 0;
  GLuint eboIndices = 0;
  GLuint indexCount = 0;

  Mesh() = default;

  ~Mesh() {
    glDeleteBuffers(1, &vboPositions);
    glDeleteBuffers(1, &vboColours);
    glDeleteBuffers(1, &vboNormals);
    glDeleteBuffers(1, &vboTexCoords);
    glDeleteBuffers(1, &eboIndices);
    glDeleteVertexArrays(1, &vao);
  }

  void setup(std::vector<GLfloat> &positions, std::vector<GLfloat> &colours,
             std::vector<GLfloat> &normals, std::vector<GLuint> &indices,
             std::vector<GLfloat> *texCoords = nullptr) {
    indexCount = indices.size();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // positions
    glGenBuffers(1, &vboPositions);
    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat),
                 positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // colours
    glGenBuffers(1, &vboColours);
    glBindBuffer(GL_ARRAY_BUFFER, vboColours);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(GLfloat),
                 colours.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // normals
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat),
                 normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // texture coordinates if provided
    if (texCoords && !texCoords->empty()) {
      glGenBuffers(1, &vboTexCoords);
      glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
      glBufferData(GL_ARRAY_BUFFER, texCoords->size() * sizeof(GLfloat),
                   texCoords->data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // indices
    glGenBuffers(1, &eboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 indices.data(), GL_STATIC_DRAW);

    // unbind vao
    glBindVertexArray(0);
  }

  void draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
};
