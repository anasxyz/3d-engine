#pragma once
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <vector>

using namespace glm;

struct Vertex {
  vec3 position;
  vec4 colour;
  vec3 normal;
  vec2 uv;

  Vertex() : position(0.0f), colour(1.0f), normal(0.0f, 1.0f, 0.0f), uv(0.0f) {}

};

/*
 * idk why the render bug is happening
 *
 * position = 12 bytes because vec3
 * colour = 16 bytes because vec4
 * normal = 12 bytes because vec3
 * uv = 8 bytes because vec2
 *
 * total = 48 bytes
 *
 */
static_assert(sizeof(Vertex) == 48, "Vertex struct must be tightly packed");
