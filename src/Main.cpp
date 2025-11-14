#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../include/glad/glad.h"

#include "../include/Camera.h"
#include "../include/GLFW/WrapperGLFW.h"
#include "../include/MeshFactory.h"
#include "../include/Scene.h"
#include "../include/TextureLoader.h"

#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_glfw.h"
#include "../external/imgui/imgui_impl_opengl3.h"

using namespace glm;
using namespace std;

GLuint program;
GLuint modelId, viewId, projectionId;

GLuint lightPositionId, viewPositionId, lightColourId, ambientStrengthId,
    specularStrengthId, shininessId;

GLuint useTextureId, texSamplerId;
GLuint crateTex, globeTex, donutTex;

GLWrapper *glw;
int windowWidth = 1024, windowHeight = 768;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// lighting
vec3 lightPosition(10.0f, 2.0f, 2.0f);
vec3 lightColour(1.0f, 1.0f, 1.0f); // white
float ambientStrength = 0.1f;
float shininess = 8.0f;
float specularStrength = 0.1f;

// scene
Scene scene;

// rotation speeds
float rotSpeedX = 0.01f;
float rotSpeedY = 0.01f;

void updateObjectMovement(Object &obj) {
  GLFWwindow *window = glw->window();

  if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    obj.transform.rotation.x += rotSpeedX;
  if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    obj.transform.rotation.x -= rotSpeedX;
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    obj.transform.rotation.y += rotSpeedY;
  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    obj.transform.rotation.y -= rotSpeedY;
}

void render() {
  static float lastFrameTime = 0.0f;
	// idk i had to set it to an intial value to start
  static float fps = 60.0f;
  float currentFrameTime = glfwGetTime();
  float deltaTime = currentFrameTime - lastFrameTime;
  lastFrameTime = currentFrameTime;

  // smoothing factor 
	// (0 < alpha <= 1)
  const float alpha = 0.1f;
  fps = fps * (1.0f - alpha) + (1.0f / deltaTime) * alpha;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
  ImGui::Begin("FPS", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBackground);
  ImGui::SetWindowFontScale(2.0f);
  ImGui::Text("FPS: %.1f", fps);
  ImGui::End();

  GLFWwindow *window = glw->window();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  glViewport(0, 0, windowWidth, windowHeight);

  glUseProgram(program);

  // projection
  float aspect = (float)windowWidth / (float)windowHeight;
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
  glUniformMatrix4fv(projectionId, 1, GL_FALSE, &projection[0][0]);

  // view
  camera.processCameraMovement(window, deltaTime);
  camera.processCameraLook(window, deltaTime);
  glm::mat4 view = camera.getViewMatrix();
  glUniformMatrix4fv(viewId, 1, GL_FALSE, &view[0][0]);

  // lighting uniforms
  glUniform3fv(lightPositionId, 1, &lightPosition[0]);
  glUniform3fv(viewPositionId, 1, &camera.position[0]);
  glUniform3fv(lightColourId, 1, &lightColour[0]);
  glUniform1f(ambientStrengthId, ambientStrength);
  glUniform1f(shininessId, shininess);
  glUniform1f(specularStrengthId, specularStrength);

  for (auto &obj : scene.objects) {
    // set model matrix
    glUniformMatrix4fv(modelId, 1, GL_FALSE, &obj->transform.getMatrix()[0][0]);

    // bind texture if one exists for this object
    if (obj->textureId != 0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, obj->textureId);
      glUniform1i(texSamplerId, 0);
      glUniform1i(useTextureId, 1);
    } else {
      glUniform1i(useTextureId, 0);
    }

    // rotate object
    obj->transform.rotation.x += rotSpeedX;
    obj->transform.rotation.y += rotSpeedY;

    // draw object mesh
    obj->mesh.draw();
  }

  glUseProgram(0);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void initImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(glw->window(), true);
  ImGui_ImplOpenGL3_Init("#version 420");
}

void init() {
  // init imgui
  initImGui();

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

  crateTex = TextureLoader::loadTexture("assets/textures/crate.png");
  std::cout << "crateTex: " << crateTex << std::endl;
  globeTex = TextureLoader::loadTexture("assets/textures/globe.jpg");
  std::cout << "globeTex: " << globeTex << std::endl;
  donutTex = TextureLoader::loadTexture("assets/textures/donut3.jpg");
  std::cout << "donutTex: " << donutTex << std::endl;

  // create scene objects
  auto cube1 = scene.createObject("Cube1", cubeMesh);
  cube1->transform.position = vec3(0.0f, 0.0f, -2.0f);
  cube1->transform.scale = vec3(0.5f, 0.3f, 0.5f);
  cube1->textureId = crateTex;

  auto torus1 = scene.createObject("Torus1", torusMesh);
  torus1->transform.position = vec3(2.0f, 1.0f, -4.0f);
  torus1->transform.scale = vec3(0.5f);
  torus1->textureId = donutTex;

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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete glw;
  } catch (const std::exception &e) {
    cerr << "Fatal Error: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
