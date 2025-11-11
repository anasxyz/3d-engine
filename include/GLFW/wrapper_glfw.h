#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <string>

class GLWrapper {
public:
  GLWrapper(int width, int height, const char *title);
  ~GLWrapper();

  void run(const std::function<void()> &renderLoop);
  void setKeyCallback(
      const std::function<void(GLFWwindow *, int, int, int, int)> &callback);

  GLuint loadShader(const char *vertexPath, const char *fragmentPath);
  GLFWwindow *window() const { return m_window; }
  void displayVersion() const;

private:
  GLFWwindow *m_window;
  int m_width, m_height;
  std::string m_title;
  std::function<void(GLFWwindow *, int, int, int, int)> m_keyCallback;

  static void internalKeyCallback(GLFWwindow *window, int key, int scancode,
                                  int action, int mods);
  static std::string readFile(const char *path);
  GLuint compileShader(GLenum type, const std::string &source);
};
