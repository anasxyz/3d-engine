#include "../include/Mesh.h"
#include "../include/MeshFactory.h"
#include "../include/GLFW/wrapper_glfw.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

using namespace glm;
using namespace std;

GLuint program;
GLuint modelID, viewID, projectionID;

GLWrapper *glw;
vec3 cameraPos(0.0f, 0.0f, 3.0f);
vec3 cameraFront(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);
float camYaw = -90.0f;
float camPitch = 0.0f;

vector<Mesh> sceneObjects;
int windowWidth = 1024, windowHeight = 768;

float rotSpeedX = 0.01f;
float rotSpeedY = 0.01f;

void updateCamera(GLFWwindow *window) {
  float moveSpeed = 0.03f;
  float rotSpeed = 1.0f;

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camYaw -= rotSpeed;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camYaw += rotSpeed;
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camPitch += rotSpeed;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camPitch -= rotSpeed;

  if (camPitch > 89.0f)
    camPitch = 89.0f;
  if (camPitch < -89.0f)
    camPitch = -89.0f;

  vec3 front;
  front.x = cos(radians(camYaw)) * cos(radians(camPitch));
  front.y = sin(radians(camPitch));
  front.z = sin(radians(camYaw)) * cos(radians(camPitch));
  cameraFront = normalize(front);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += moveSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= moveSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= normalize(cross(cameraFront, cameraUp)) * moveSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += normalize(cross(cameraFront, cameraUp)) * moveSpeed;
}

void render() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwGetFramebufferSize(glw->window(), &windowWidth, &windowHeight);
  glViewport(0, 0, windowWidth, windowHeight);

  glUseProgram(program);

  float aspect = (float)windowWidth / (float)windowHeight;
  mat4 projection = perspective(radians(45.0f), aspect, 0.1f, 100.0f);
  mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

  glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);

  // draw all scene objects
  for (auto &obj : sceneObjects) {
    obj.transform.rotation.x += rotSpeedX;
    obj.transform.rotation.y += rotSpeedY;
    mat4 model = obj.transform.getMatrix();
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
    obj.draw();
  }

  glUseProgram(0);
  updateCamera(glw->window());
}

void init() {
  program = glw->loadShader("shaders/lab2.vert", "shaders/lab2.frag");

  modelID = glGetUniformLocation(program, "model");
  viewID = glGetUniformLocation(program, "view");
  projectionID = glGetUniformLocation(program, "projection");

  // Build the scene
  Mesh cube1 = createCube();
  cube1.transform.position = vec3(0.0f, 0.0f, -2.0f);

  Mesh cube2 = createCube();
  cube2.transform.position = vec3(2.0f, 1.0f, -4.0f);
  cube2.transform.scale = vec3(0.5f);

  Mesh cube3 = createCube();
  cube3.transform.position = vec3(-2.0f, -1.0f, -3.0f);
  cube3.transform.scale = vec3(0.8f);

  sceneObjects = {cube1, cube2, cube3};
}

int main() {
  try {
    glw = new GLWrapper(windowWidth, windowHeight, "Project");

    glw->setKeyCallback(
        [](GLFWwindow *window, int key, int scancode, int action, int mods) {
          if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        });

    glw->displayVersion();
    init();
    glw->run(render);
    delete glw;

  } catch (const std::exception &e) {
    cerr << "Fatal Error: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
