/*
 Lab2start.cpp
 Creates a cube and defines a uniform variable to pass a transformation
 to the vertx shader.
 Use this example as a start to lab2 or extract bits and add to
 an example of your own to practice working with 3D transformations
 and uniform variables.
 Iain Martin October 2022
*/

/* Link to static libraries, could define these as linker inputs in the project
settings instead if you prefer */
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include "../include/GLFW/wrapper_glfw.h"
#include <iostream>

/* GLM headers */
#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow *g_window = nullptr; // global window pointer

GLuint positionBufferObject, colourBufferObject, indexBufferObject;
GLuint vao;
GLuint program;
GLuint modelID, viewID, projectionID;

/* position and view globals */
GLfloat angle_x, angle_x_inc;
GLfloat angle_y, angle_y_inc;
GLfloat angle_z, angle_z_inc;

using namespace std;
using namespace glm;

// cube global variables
vec3 cubePosition(0.0f, 0.0f, -1.0f);
vec3 cubeScale(1.0f, 1.0f, 1.0f);

// camera variables
vec3 cameraPosition(0.0f, 0.0f, 2.0f);
vec3 cameraFront(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);
float camYaw = -90.0f; // start facing toward -Z
float camPitch = 0.0f;

// window global variables
int width_g = 1024, height_g = 768;
float aspect;

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper *glw) {
  angle_x = angle_y = angle_z = 0.0f;
  angle_x_inc = angle_y_inc = angle_z_inc = 0.0f;

  // Vertex array object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // 8 unique cube vertices
  GLfloat vertices[] = {
      -0.5f, -0.5f, -0.5f, 1.f, // 0
      0.5f,  -0.5f, -0.5f, 1.f, // 1
      0.5f,  0.5f,  -0.5f, 1.f, // 2
      -0.5f, 0.5f,  -0.5f, 1.f, // 3
      -0.5f, -0.5f, 0.5f,  1.f, // 4
      0.5f,  -0.5f, 0.5f,  1.f, // 5
      0.5f,  0.5f,  0.5f,  1.f, // 6
      -0.5f, 0.5f,  0.5f,  1.f  // 7
  };

  GLfloat colors[] = {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1,
                      1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1};

  // Indices for 12 triangles (2 per face)
  GLuint indices[] = {
      0, 1, 2, 2, 3, 0, // back
      4, 5, 6, 6, 7, 4, // front
      0, 4, 7, 7, 3, 0, // left
      1, 5, 6, 6, 2, 1, // right
      3, 2, 6, 6, 7, 3, // top
      0, 1, 5, 5, 4, 0  // bottom
  };

  glEnable(GL_DEPTH_TEST);

  // VBO for positions
  glGenBuffers(1, &positionBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // VBO for colors
  glGenBuffers(1, &colourBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, colourBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

  // EBO for indices
  glGenBuffers(1, &indexBufferObject);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Load shaders
  try {
    program = glw->LoadShader("shaders/lab2.vert", "shaders/lab2.frag");
  } catch (exception &e) {
    cout << "Caught exception: " << e.what() << endl;
    cin.ignore();
    exit(0);
  }

  // Uniforms
  modelID = glGetUniformLocation(program, "model");
  viewID = glGetUniformLocation(program, "view");
  projectionID = glGetUniformLocation(program, "projection");
}

void updatePosition() {
  float moveSpeed = 0.01f;
  if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS)
    cubePosition.y += moveSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS)
    cubePosition.y -= moveSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS)
    cubePosition.x -= moveSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    cubePosition.x += moveSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    cubePosition.z += moveSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    cubePosition.z -= moveSpeed;
}

void updateScale() {
  float scaleSpeed = 0.01f;
  if (glfwGetKey(g_window, GLFW_KEY_J) == GLFW_PRESS)
    cubeScale += glm::vec3(scaleSpeed);
  if (glfwGetKey(g_window, GLFW_KEY_K) == GLFW_PRESS)
    cubeScale -= glm::vec3(scaleSpeed);
}

void updateRotation() {
  float rotSpeed = 0.01f;
  if (glfwGetKey(g_window, GLFW_KEY_Y) == GLFW_PRESS)
    angle_x_inc += rotSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_U) == GLFW_PRESS)
    angle_x_inc -= rotSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_H) == GLFW_PRESS)
    angle_y_inc += rotSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_J) == GLFW_PRESS)
    angle_y_inc -= rotSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_N) == GLFW_PRESS)
    angle_z_inc += rotSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_M) == GLFW_PRESS)
    angle_z_inc -= rotSpeed;
}

void updateCamera() {
  float moveSpeed = 0.02f;
  float rotSpeed = 1.0f;

  // rotation (arrow keys)
  if (glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camYaw -= rotSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camYaw += rotSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS)
    camPitch += rotSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camPitch -= rotSpeed;

  // clamp pitch to avoid flipping
  if (camPitch > 89.0f)
    camPitch = 89.0f;
  if (camPitch < -89.0f)
    camPitch = -89.0f;

  // update cameraFront from yaw and pitch
  vec3 front;
  front.x = cos(radians(camYaw)) * cos(radians(camPitch));
  front.y = sin(radians(camPitch));
  front.z = sin(radians(camYaw)) * cos(radians(camPitch));
  cameraFront = normalize(front);

  // movement (wasd)
  if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPosition += moveSpeed * cameraFront;
  if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPosition -= moveSpeed * cameraFront;
  if (glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPosition -= normalize(cross(cameraFront, cameraUp)) * moveSpeed;
  if (glfwGetKey(g_window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPosition += normalize(cross(cameraFront, cameraUp)) * moveSpeed;
}

// Called to update the display.
// You should call glfwSwapBuffers() after all of your rendering to display what
// you rendered.
void display() {
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwGetFramebufferSize(g_window, &width_g, &height_g);
  aspect = (float)width_g / height_g;
  glViewport(0, 0, width_g, height_g);

  glUseProgram(program);

  glBindVertexArray(vao);

  // positions
  glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  // colors
  glBindBuffer(GL_ARRAY_BUFFER, colourBufferObject);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

  // bind EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

  updateCamera();

  mat4 model = mat4(1.0f);
  model = translate(model, cubePosition);
  model = rotate(model, -angle_x, vec3(1, 0, 0));
  model = rotate(model, -angle_y, vec3(0, 1, 0));
  model = rotate(model, -angle_z, vec3(0, 0, 1));
  model = scale(model, cubeScale);

  mat4 view =
      glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
  mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

  glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
  glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);

  // Draw indexed triangles
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glUseProgram(0);

  angle_x += angle_x_inc;
  angle_y += angle_y_inc;
  angle_z += angle_z_inc;
}
/* Called whenever the window is resized. The new window size is given, in
 * pixels. */
static void reshape(GLFWwindow *window, int w, int h) {
  glfwGetFramebufferSize(window, &width_g, &height_g);
  glViewport(0, 0, width_g, height_g);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow *window, int k, int s, int action,
                        int mods) {
  if (action != GLFW_PRESS)
    return;

  if (k == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

/* An error callback function to output GLFW errors*/
static void error_callback(int error, const char *description) {
  fputs(description, stderr);
}

/* Entry point of program */
int main(int argc, char *argv[]) {
  GLWrapper *glw = new GLWrapper(width_g, height_g, "Project");
  g_window = glw->getWindow();

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD. Exiting." << endl;
    return -1;
  }

  /* Note it you might want to move this call to the wrapper class */
  glw->setErrorCallback(error_callback);

  glw->setRenderer(display);
  glw->setKeyCallback(keyCallback);
  glw->setReshapeCallback(reshape);

  // Output version
  glw->DisplayVersion();

  init(glw);

  glw->eventLoop();

  delete (glw);
  return 0;
}
