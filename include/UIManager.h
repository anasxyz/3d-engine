#pragma once

#include "GLFW/WrapperGLFW.h"
#include <glm/glm.hpp>

class UIManager {
public:
  void init(GLFWwindow *window);

  void beginFrame();

  void renderFPS(float fps, bool &showControls);
  void renderControls(bool &showControls);
	void renderCameraInfo(const glm::vec3 &position, float &yaw, float &pitch);

  void endFrame();
};
