#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace glm;

struct Transform {
  vec3 position = vec3(0.0f);
  vec3 rotation = vec3(0.0f);
  vec3 scale = vec3(1.0f);

  mat4 getMatrix() const {
		// translation
    mat4 m = translate(mat4(1.0f), position);

		// rotation
    m = rotate(m, rotation.x, vec3(1, 0, 0));
    m = rotate(m, rotation.y, vec3(0, 1, 0));
    m = rotate(m, rotation.z, vec3(0, 0, 1));

		// scale
		// for some reason can't do scale() without glm:: ??
    m = glm::scale(m, scale);

    return m;
  }
};

