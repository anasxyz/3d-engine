#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb/stb_image.h"

#include "../include/GLFW/wrapper_glfw.h"
#include "../include/MeshFactory.h"
#include "../include/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;
using namespace std;

GLuint program;
GLuint modelId, viewId, projectionId;
GLuint lightPositionId, viewPositionId, lightColourId, ambientStrengthId,
    specularStrengthId, shininessId;
GLuint crateTex, globeTex, useTextureId, texSamplerId;

GLWrapper *glw;
int windowWidth = 1024, windowHeight = 768;

// camera
vec3 cameraPos(0.0f, 0.0f, 3.0f);
vec3 cameraFront(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);
float camYaw = -90.0f;
float camPitch = 0.0f;

// lighting
vec3 lightPosition(60.0f, 2.0f, 2.0f);
vec3 lightColour(1.0f, 1.0f, 1.0f); // white
float ambientStrength = 0.1f;
float shininess = 8.0f;
float specularStrength = 0.1f;

// scene
Scene scene;

// rotation speeds
float rotSpeedX = 0.0003f;
float rotSpeedY = 0.0003f;

void updateCamera() {
  GLFWwindow *window = glw->window();

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
  GLFWwindow *window = glw->window();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwGetFramebufferSize(glw->window(), &windowWidth, &windowHeight);
  glViewport(0, 0, windowWidth, windowHeight);

  glUseProgram(program);

  float aspect = (float)windowWidth / (float)windowHeight;
  mat4 projection = perspective(radians(45.0f), aspect, 0.1f, 100.0f);
  mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

  glUniformMatrix4fv(viewId, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projectionId, 1, GL_FALSE, &projection[0][0]);

  glUniform3fv(lightPositionId, 1, &lightPosition[0]);
  glUniform3fv(viewPositionId, 1, &cameraPos[0]);
  glUniform3fv(lightColourId, 1, &lightColour[0]);
  glUniform1f(ambientStrengthId, ambientStrength);
  glUniform1f(shininessId, shininess);
  glUniform1f(specularStrengthId, specularStrength);

  for (auto &obj : scene.objects) {
    // set model matrix
    glUniformMatrix4fv(modelId, 1, GL_FALSE, &obj->transform.getMatrix()[0][0]);

    // bind object texture if it exists
    if (obj->textureId != 0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, obj->textureId);
      glUniform1i(texSamplerId, 0);
      glUniform1i(useTextureId, 1);
    } else {
      glUniform1i(useTextureId, 0);
    }

    obj->transform.rotation.x += rotSpeedX;
    obj->transform.rotation.y += rotSpeedY;

    obj->mesh.draw();
  }

  glUseProgram(0);

  updateCamera();
}

GLuint loadTexture(const std::string &path) {
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  if (!data) {
    std::cerr << "Failed to load texture: " << path << std::endl;
    return 0;
  }

  GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return textureID;
}

void init() {
  // load shaders
  program = glw->loadShader("shaders/vs.vert", "shaders/fs.frag");

  // uniform locations
  modelId = glGetUniformLocation(program, "model");
  viewId = glGetUniformLocation(program, "view");
  projectionId = glGetUniformLocation(program, "projection");

  lightPositionId = glGetUniformLocation(program, "lightPosition");
  viewPositionId = glGetUniformLocation(program, "viewPosition");
  lightColourId = glGetUniformLocation(program, "lightColour");
  ambientStrengthId = glGetUniformLocation(program, "ambientStrength");
  specularStrengthId = glGetUniformLocation(program, "specularStrength");
  shininessId = glGetUniformLocation(program, "shininess");

  useTextureId = glGetUniformLocation(program, "useTexture");
  texSamplerId = glGetUniformLocation(program, "texSampler");

  // create cube meshes
  Mesh cubeMesh = createCube();
  Mesh sphereMesh = createSphere();
  Mesh torusMesh = createTorus();

  crateTex = loadTexture("assets/textures/crate.png");
  std::cout << "crateTex: " << crateTex << std::endl;
	globeTex = loadTexture("assets/textures/globe.jpg");
	std::cout << "globeTex: " << globeTex << std::endl;

  // create scene objects
  auto cube1 = scene.createObject("Cube1", cubeMesh);
  cube1->transform.position = vec3(0.0f, 0.0f, -2.0f);
  cube1->transform.scale = vec3(0.5f, 0.3f, 0.5f);
  cube1->textureId = crateTex;

  auto torus1 = scene.createObject("Torus1", torusMesh);
  torus1->transform.position = vec3(2.0f, 1.0f, -4.0f);
  torus1->transform.scale = vec3(0.5f);
	torus1->textureId = crateTex;

  auto sphere1 = scene.createObject("Sphere1", sphereMesh);
  sphere1->transform.position = vec3(-2.0f, -1.0f, -3.0f);
  sphere1->transform.scale = vec3(0.8f);
	sphere1->textureId = globeTex;
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
