#pragma once
#include "Mesh.h"
#include <string>

using namespace glm;

class ObjectLoader {
public:
  static Mesh loadOBJModel(const std::string &objectFilename, const vec4 &defaultColour);
};
