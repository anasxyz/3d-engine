#pragma once
#include "Mesh.h"

inline Mesh createCube(float size = 1.0f) {
  Mesh cube;
  float s = size * 0.5f;

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  // positions
  glm::vec3 positions[] = {
      {-s, -s, -s}, {s, -s, -s}, {s, s, -s}, {-s, s, -s}, // back
      {-s, -s, s},  {s, -s, s},  {s, s, s},  {-s, s, s},  // front
      {-s, -s, -s}, {-s, s, -s}, {-s, s, s}, {-s, -s, s}, // left
      {s, -s, -s},  {s, s, -s},  {s, s, s},  {s, -s, s},  // right
      {-s, s, -s},  {s, s, -s},  {s, s, s},  {-s, s, s},  // top
      {-s, -s, -s}, {s, -s, -s}, {s, -s, s}, {-s, -s, s}  // bottom
  };

  glm::vec3 normals[] = {
      {0, 0, -1}, // back
      {0, 0, 1},  // front
      {-1, 0, 0}, // left
      {1, 0, 0},  // right
      {0, 1, 0},  // top
      {0, -1, 0}  // bottom
  };

  glm::vec2 uv[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

  for (int face = 0; face < 6; face++) {
    glm::vec3 normal = normals[face];
    for (int i = 0; i < 4; i++) {
      vertices.emplace_back(positions[face * 4 + i], normal, uv[i]);
    }
  }

  GLuint idx[] = {0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,
                  8,  9,  10, 10, 11, 8,  12, 13, 14, 14, 15, 12,
                  16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

  indices.assign(idx, idx + 36);

  cube.setup(vertices, indices);
  return cube;
}

inline Mesh createSphere(float radius = 1.0f, int stacks = 32,
                         int slices = 64) {
  Mesh sphere;

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  for (int i = 0; i <= stacks; i++) {
    float v = float(i) / stacks;
    float phi = glm::pi<float>() * v;

    float sinPhi = sin(phi);
    float cosPhi = cos(phi);

    for (int j = 0; j <= slices; j++) {
      float u = float(j) / slices;
      float theta = glm::two_pi<float>() * u;

      float x = radius * sinPhi * cos(theta);
      float y = radius * cosPhi;
      float z = radius * sinPhi * sin(theta);

      glm::vec3 pos(x, y, z);
      glm::vec3 normal = glm::normalize(pos);

      vertices.emplace_back(pos, normal, glm::vec2(u, 1.0f - v));
    }
  }

  for (int i = 0; i < stacks; i++) {
    for (int j = 0; j < slices; j++) {

      int first = i * (slices + 1) + j;
      int second = first + slices + 1;

      indices.push_back(first);
      indices.push_back(second);
      indices.push_back(first + 1);

      indices.push_back(second);
      indices.push_back(second + 1);
      indices.push_back(first + 1);
    }
  }

  sphere.setup(vertices, indices);
  return sphere;
}

inline Mesh createTorus(float majorRadius = 1.0f, float minorRadius = 0.5f,
                        int majorSegments = 64, int minorSegments = 32) {
  Mesh torus;

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  for (int i = 0; i <= majorSegments; i++) {
    float u = float(i) / majorSegments;
    float theta = glm::two_pi<float>() * u;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    for (int j = 0; j <= minorSegments; j++) {
      float v = float(j) / minorSegments;
      float phi = glm::two_pi<float>() * v;

      float cosPhi = cos(phi);
      float sinPhi = sin(phi);

      float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
      float y = minorRadius * sinPhi;
      float z = (majorRadius + minorRadius * cosPhi) * sinTheta;

      glm::vec3 pos(x, y, z);

      glm::vec3 center(majorRadius * cosTheta, 0, majorRadius * sinTheta);
      glm::vec3 normal = glm::normalize(pos - center);

      vertices.emplace_back(pos, normal, glm::vec2(u, v));
    }
  }

  for (int i = 0; i < majorSegments; i++) {
    for (int j = 0; j < minorSegments; j++) {

      int first = i * (minorSegments + 1) + j;
      int second = first + minorSegments + 1;

      indices.push_back(first);
      indices.push_back(second);
      indices.push_back(first + 1);

      indices.push_back(second);
      indices.push_back(second + 1);
      indices.push_back(first + 1);
    }
  }

  torus.setup(vertices, indices);
  return torus;
}

inline Mesh
createSkybox(float radius = 1.0f,
             int stacks = 64, // Increased resolution for better quality
             int slices = 128) {
  Mesh sphere;

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  for (int i = 0; i <= stacks; i++) {
    float v_ratio = float(i) / stacks; // normalized V from 0 to 1
    float phi = v_ratio *
                glm::pi<float>(); // latitude: 0 (top pole) to pi (bottom pole)

    float sinPhi = sin(phi);
    float cosPhi = cos(phi);

    for (int j = 0; j <= slices; j++) {
      float u_ratio = float(j) / slices;            // normalized U from 0 to 1
      float theta = u_ratio * glm::two_pi<float>(); // longitude: 0 to 2*pi

      // Cartesian coordinates from spherical coordinates
      float x = radius * sinPhi * cos(theta);
      float y = radius * cosPhi;
      float z = radius * sinPhi * sin(theta);

      glm::vec3 pos(x, y, z);
      glm::vec3 normal =
          glm::normalize(pos); // Normal is same as position for a sphere

      // Equirectangular UV mapping:
      // U: (atan2(z, x) / (2 * PI)) + 0.5
      // V: (asin(y / radius) / PI) + 0.5
      // However, since `u_ratio` and `v_ratio` already effectively map 0-1
      // across the longitude/latitude, we can directly use them. The `1.0f -
      // v_ratio` is crucial for vertical orientation.
      glm::vec2 uv(u_ratio, 1.0f - v_ratio); // 1.0f - v_ratio to flip V if
                                             // texture origin is bottom-left

      vertices.emplace_back(pos, normal, uv);
    }
  }

  // Indices generation (same as before, it's correct for a UV-mapped sphere)
  for (int i = 0; i < stacks; i++) {
    for (int j = 0; j < slices; j++) {
      int first = i * (slices + 1) + j;
      int second = first + slices + 1;

      indices.push_back(first);
      indices.push_back(second);
      indices.push_back(first + 1);

      indices.push_back(second);
      indices.push_back(second + 1);
      indices.push_back(first + 1);
    }
  }

  sphere.setup(vertices, indices);
  return sphere;
}
