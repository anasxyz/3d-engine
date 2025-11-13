#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw;
  float pitch;

  float movementSpeed;
  float mouseSensitivity;

  Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f),
         glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f),
         float startYaw = -90.0f, float startPitch = 0.0f);

  glm::mat4 getViewMatrix() const;

  void processCameraMovement(GLFWwindow *window, float deltaTime);

  void processCameraLook(GLFWwindow *window, float rotSpeed);

private:
  void updateCameraVectors();
};
