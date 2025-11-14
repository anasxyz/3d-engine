#include "../include/GLFW/WrapperGLFW.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

GLWrapper::GLWrapper(int width, int height, const char *title)
    : m_width(width), m_height(height), m_title(title), m_window(nullptr) {
  if (!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);

  m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!m_window)
    throw std::runtime_error("Failed to create GLFW window");

  glfwMakeContextCurrent(m_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::runtime_error("Failed to initialize GLAD");

  glfwSetWindowUserPointer(m_window, this);
  glfwSetKeyCallback(m_window, internalKeyCallback);

  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}

GLWrapper::~GLWrapper() { glfwTerminate(); }

void GLWrapper::run(const std::function<void()> &renderLoop) {
  while (!glfwWindowShouldClose(m_window)) {
    renderLoop();
    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
}

void GLWrapper::setKeyCallback(
    const std::function<void(GLFWwindow *, int, int, int, int)> &callback) {
  m_keyCallback = callback;
}

void GLWrapper::internalKeyCallback(GLFWwindow *window, int key, int scancode,
                                    int action, int mods) {
  auto wrapper = static_cast<GLWrapper *>(glfwGetWindowUserPointer(window));
  if (wrapper && wrapper->m_keyCallback)
    wrapper->m_keyCallback(window, key, scancode, action, mods);
}

std::string GLWrapper::readFile(const char *path) {
  std::ifstream file(path);
  if (!file)
    throw std::runtime_error(std::string("Cannot open shader file: ") + path);
  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

GLuint GLWrapper::compileShader(GLenum type, const std::string &source) {
  GLuint shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    std::vector<char> log(len);
    glGetShaderInfoLog(shader, len, nullptr, log.data());
    throw std::runtime_error("Shader compile error: " +
                             std::string(log.data()));
  }

  return shader;
}

GLuint GLWrapper::loadShader(const char *vertexPath, const char *fragmentPath) {
  std::string vertexSrc = readFile(vertexPath);
  std::string fragmentSrc = readFile(fragmentPath);

  GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
  GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);

  GLint linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    GLint len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    std::vector<char> log(len);
    glGetProgramInfoLog(program, len, nullptr, log.data());
    throw std::runtime_error("Program link error: " + std::string(log.data()));
  }

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  return program;
}

void GLWrapper::displayVersion() const {
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  std::cout << "OpenGL Version: " << major << "." << minor << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
}
