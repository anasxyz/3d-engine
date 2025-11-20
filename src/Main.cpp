#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../include/glad/glad.h"

#include "../include/Camera.h"
#include "../include/GLFW/WrapperGLFW.h"
#include "../include/MeshFactory.h"
#include "../include/ObjectLoader.h"
#include "../include/Scene.h"
#include "../include/Skybox.h"
#include "../include/TextureLoader.h"
#include "../include/UIManager.h"

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
GLuint crateTex, donutTex;
GLuint mercuryTex, venusTex, earthTex, marsTex, jupiterTex, saturnTex,
    uranusTex, neptuneTex, plutoTex;

// controls
// TODO: move this stuff to it's own separate area
// initially hidden
static bool showControls = false;
static bool hPressedLastFrame = false;
static float lastToggleTime = 0.0f;
const float toggleCooldown = 0.3f;

// fps stuff
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

// window stuff
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
// skybox
Skybox *skybox;
// UI manager
UIManager ui;

// rotation speeds
float rotSpeed = 0.2f;

void updateObjectMovement(Object &obj) {
  GLFWwindow *window = glw->window();

  if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
    obj.transform.rotation.x += 2 * rotSpeed * deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
    obj.transform.rotation.y -= 2 * rotSpeed * deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    obj.transform.rotation.z -= 2 * rotSpeed * deltaTime;
  }

  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    obj.transform.position.y += 1.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    obj.transform.position.x -= 1.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    obj.transform.position.y -= 1.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    obj.transform.position.x += 1.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    obj.transform.position.z += 1.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    obj.transform.position.z -= 1.0f * deltaTime;
}

void render() {
  GLFWwindow *window = glw->window();

  // fps calculation
  static float fps = 60.0f;
  float currentFrameTime = glfwGetTime();
  deltaTime = currentFrameTime - lastFrameTime;
  lastFrameTime = currentFrameTime;
  const float alpha = 0.05f;
  fps = fps * (1.0f - alpha) + (1.0f / deltaTime) * alpha;

  bool hPressed = glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS;
  float currentTime = glfwGetTime();

  // ui stuff
  if (hPressed && !hPressedLastFrame &&
      (currentTime - lastToggleTime) > toggleCooldown) {
    showControls = !showControls;
    lastToggleTime = currentTime;
  }
  hPressedLastFrame = hPressed;

  ui.beginFrame();
  ui.renderFPS(fps);
	ui.renderControls(showControls);
  ui.endFrame();

  // clear background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  glViewport(0, 0, windowWidth, windowHeight);

  float aspect = (float)windowWidth / (float)windowHeight;
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

  camera.processCameraMovement(window, deltaTime);
  camera.processCameraLook(window, deltaTime);
  glm::mat4 view = camera.getViewMatrix();

  skybox->render(view, projection);

  glUseProgram(program);

  glUniformMatrix4fv(viewId, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projectionId, 1, GL_FALSE, &projection[0][0]);

  glUniform3fv(lightPositionId, 1, &lightPosition[0]);
  glUniform3fv(viewPositionId, 1, &camera.position[0]);
  glUniform3fv(lightColourId, 1, &lightColour[0]);
  glUniform1f(ambientStrengthId, ambientStrength);
  glUniform1f(shininessId, shininess);
  glUniform1f(specularStrengthId, specularStrength);

  for (auto &obj : scene.objects) {
    glUniformMatrix4fv(modelId, 1, GL_FALSE, &obj->transform.getMatrix()[0][0]);

    if (obj->textureId != 0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, obj->textureId);
      glUniform1i(texSamplerId, 0);
      glUniform1i(useTextureId, 1);
    } else {
      glUniform1i(useTextureId, 0);
    }

    obj->transform.rotation.y += rotSpeed * deltaTime;
    obj->transform.rotation.x += rotSpeed * 0.1f * deltaTime;

    obj->mesh.draw();
  }

  glUseProgram(0);

  updateObjectMovement(*scene.objects[0]);

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
  // init ui
  ui.init(glw->window());

  // load main shaders
  program = glw->loadShader("shaders/vs.vert", "shaders/fs.frag");

  // skybox
  std::vector<std::string> faces;
  faces.push_back("space2/px.png"); // +X
  faces.push_back("space2/nx.png"); // -X
  faces.push_back("space2/py.png"); // +Y
  faces.push_back("space2/ny.png"); // -Y
  faces.push_back("space2/pz.png"); // +Z
  faces.push_back("space2/nz.png"); // -Z

  skybox = new Skybox(glw, faces);

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

  // create premade meshes
  Mesh cubeMesh = createCube();
  Mesh sphereMesh = createSphere();
  Mesh torusMesh = createTorus();

  // load textures
  crateTex = TextureLoader::loadTexture("crate.png");
  donutTex = TextureLoader::loadTexture("donut3.jpg");
  earthTex = TextureLoader::loadTexture("planets/earth_diffuse.jpg");

  // create scene objects
  auto cube1 = scene.createObject("Cube1", cubeMesh);
  cube1->transform.position = vec3(0.0f, 0.0f, -2.0f);
  cube1->transform.scale = vec3(0.5f, 0.3f, 0.5f);

  auto torus1 = scene.createObject("Torus1", torusMesh);
  torus1->transform.position = vec3(2.0f, 1.0f, -4.0f);
  torus1->transform.scale = vec3(0.5f);
  torus1->textureId = donutTex;

  auto sphere1 = scene.createObject("Sphere1", sphereMesh);
  sphere1->transform.position = vec3(-2.0f, -1.0f, -3.0f);
  sphere1->transform.scale = vec3(0.8f);
  sphere1->textureId = earthTex;

  auto car =
      ObjectLoader::loadOBJObject("Car.obj", vec4(0.0f, 0.0f, 0.0f, 1.0f));
  scene.addObject(car);
  car->transform.position = vec3(10.0f, 0.0f, -10.0f);
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
