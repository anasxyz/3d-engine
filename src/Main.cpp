#include "../include/GLFW/wrapper_glfw.h"
#include "../include/MeshFactory.h"
#include "../include/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

using namespace glm;
using namespace std;

GLuint program;
GLuint modelID, viewID, projectionID;

GLWrapper *glw;
int windowWidth = 1024, windowHeight = 768;

// camera
vec3 cameraPos(0.0f, 0.0f, 3.0f);
vec3 cameraFront(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);
float camYaw = -90.0f;
float camPitch = 0.0f;

// scene
Scene scene;

// rotation speeds
float rotSpeedX = 0.01f;
float rotSpeedY = 0.01f;

void updateCamera(GLFWwindow *window) {
  float moveSpeed = 0.03f;
  float rotSpeed = 1.0f;

  // camera rotation
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camYaw -= rotSpeed;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camYaw += rotSpeed;
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camPitch += rotSpeed;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camPitch -= rotSpeed;

  camPitch = clamp(camPitch, -89.0f, 89.0f);

  vec3 front;
  front.x = cos(radians(camYaw)) * cos(radians(camPitch));
  front.y = sin(radians(camPitch));
  front.z = sin(radians(camYaw)) * cos(radians(camPitch));
  cameraFront = normalize(front);

  // movement
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

  // animate rotation
  for (auto &obj : scene.objects) {
    obj->transform.rotation.x += rotSpeedX;
    obj->transform.rotation.y += rotSpeedY;
  }

  // draw all objects
  scene.draw(modelID);

  glUseProgram(0);
  updateCamera(glw->window());
}

void init() {
  // load shaders
  program = glw->loadShader("shaders/vs.vert", "shaders/fs.frag");

  // uniform locations
  modelID = glGetUniformLocation(program, "model");
  viewID = glGetUniformLocation(program, "view");
  projectionID = glGetUniformLocation(program, "projection");

  // create cube meshes
  Mesh cubeMesh = createCube();

  // create scene objects
  auto cube1 = scene.createObject("Cube1", cubeMesh);
  cube1->transform.position = vec3(0.0f, 0.0f, -2.0f);

  auto cube2 = scene.createObject("Cube2", cubeMesh);
  cube2->transform.position = vec3(2.0f, 1.0f, -4.0f);
  cube2->transform.scale = vec3(0.5f);

  auto cube3 = scene.createObject("Cube3", cubeMesh);
  cube3->transform.position = vec3(-2.0f, -1.0f, -3.0f);
  cube3->transform.scale = vec3(0.8f);
}

int main() {
  try {
    glw = new GLWrapper(windowWidth, windowHeight, "Scene Example");

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
