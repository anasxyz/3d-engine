#pragma once

#include "GLFW/WrapperGLFW.h"
#include <glm/glm.hpp>

class UIManager {
public:
  void init(GLFWwindow *window);
  void beginFrame();
  void renderFPS(float fps);
  void renderControls(bool &showControls);
  void endFrame();
};
