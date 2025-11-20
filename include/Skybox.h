#pragma once

#include "GLFW/WrapperGLFW.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <string>

class Skybox {
public:
  Skybox(GLWrapper *glw, const std::vector<std::string> &faces);
  void render(const glm::mat4 &view, const glm::mat4 &projection);

private:
  GLuint program;
  GLuint viewId, projectionId, samplerId;
  GLuint cubemapTexture;
  Mesh cubeMesh;
};
