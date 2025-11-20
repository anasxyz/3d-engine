#pragma once
#include "glad/glad.h"
#include <iostream>
#include <string>
#include <vector>

class TextureLoader {
public:
  static GLuint loadTexture(const std::string &path);
  static GLuint loadCubemap(const std::vector<std::string> &faces);
};
