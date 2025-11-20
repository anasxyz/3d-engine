#include "../include/Camera.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 startPosition, glm::vec3 upVector, float startYaw,
               float startPitch)
    : position(startPosition), worldUp(upVector), yaw(startYaw),
      pitch(startPitch), movementSpeed(30.0f), sensitivity(75.0f) {
  updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position, position + front, up);
}

void Camera::processCameraMovement(GLFWwindow *window, float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    position += front * velocity;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    position -= front * velocity;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    position -= right * velocity;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    position += right * velocity;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += worldUp * velocity;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		position -= worldUp * velocity;
}

void Camera::processCameraLook(GLFWwindow *window, float deltaTime) {
  float velocity = sensitivity * deltaTime;

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    yaw -= velocity;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    yaw += velocity;
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    pitch += velocity;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    pitch -= velocity;

  // clamp pitch
  pitch = glm::clamp(pitch, -89.0f, 89.0f);

  updateCameraVectors();
}
void Camera::updateCameraVectors() {
  glm::vec3 f;
  f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  f.y = sin(glm::radians(pitch));
  f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(f);

  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}
