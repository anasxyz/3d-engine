#include "../include/Mesh.h"

void Mesh::setup(const std::vector<Vertex> &vertices,
                 const std::vector<GLuint> &indices) {
  indexCount = static_cast<GLuint>(indices.size());

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // vbo
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  // ebo
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
               indices.data(), GL_STATIC_DRAW);

  // layout
  // position: location 0
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
	// std::cout << "offsetof(Vertex, position): " << offsetof(Vertex, position) << std::endl;

  // colour : location 1
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, colour));
	// std::cout << "offsetof(Vertex, colour): " << offsetof(Vertex, colour) << std::endl;

  // normal: location 2
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
	// std::cout << "offsetof(Vertex, normal): " << offsetof(Vertex, normal) << std::endl;

  // uv: location 3
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, uv));
	// std::cout << "offsetof(Vertex, uv): " << offsetof(Vertex, uv) << std::endl;

  glBindVertexArray(0);
}

void Mesh::draw() const {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
