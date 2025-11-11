#pragma once
#include "Mesh.h"
#include "Transform.h"
#include <string>

class Object {
public:
  std::string name;
  Mesh mesh;
  Transform transform;

  Object(const std::string &name, const Mesh &mesh)
      : name(name), mesh(mesh) {}

  void draw(GLuint modelUniform) const {
    glm::mat4 model = transform.getMatrix();
    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, &model[0][0]);
    mesh.draw();
  }
};
