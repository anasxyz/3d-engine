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

  Vertex() = default;
  Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t)
      : position(p), colour(1.0f, 1.0f, 1.0f, 1.0f), normal(n), uv(t) {}
};

/*
 * idk why the render bug is happening.
 *
 * position = 12 bytes because vec3
 * colour = 16 bytes because vec4
 * normal = 12 bytes because vec3
 * uv = 8 bytes because vec2
 *
 * total = 48 bytes
 *
 * okay I know the cause now, when I define vec4 colour under
 * vec3 position the bug happens, when I define it under vec2 uv (all
 * the way at the bottom) it works fine, but I dont know why.
 *
 * Suddenly it doesn't matter where I define colour anymore, it just
 * works.
 *
 */
static_assert(sizeof(Vertex) == 48, "Vertex struct must be tightly packed");
