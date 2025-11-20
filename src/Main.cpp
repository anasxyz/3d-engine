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
#include "../include/TextureManager.h"
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

GLuint isLightSourceId;

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
Camera camera(vec3(50.0f, 0.0f, 50.0f));

// lighting
vec3 lightPosition(0.0f, 0.0f, 0.0f);
vec3 lightColour(1.0f, 1.0f, 1.0f); // white
float ambientStrength = 0.15f;
float shininess = 1.0f;
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
  ui.renderFPS(fps, showControls);
  ui.renderControls(showControls);
  ui.renderCameraInfo(camera.position, camera.yaw, camera.pitch);

  // clear background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  glViewport(0, 0, windowWidth, windowHeight);

  float aspect = (float)windowWidth / (float)windowHeight;
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);

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

    // if current object is the sun
    bool isSun = (obj->name == "sun");
    // set uniform flagg
    glUniform1i(isLightSourceId, isSun ? 1 : 0);

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

  ui.endFrame();
}

void getUniformLocations() {
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

  isLightSourceId = glGetUniformLocation(program, "isLightSource");
}

void init() {
  // init ui
  ui.init(glw->window());

  // load main shaders
  program = glw->loadShader("shaders/vs.vert", "shaders/fs.frag");

  getUniformLocations();

  // skybox
  std::vector<std::string> faces;
  faces.push_back("space3/px.png"); // +X
  faces.push_back("space3/nx.png"); // -X
  faces.push_back("space3/py.png"); // +Y
  faces.push_back("space3/ny.png"); // -Y
  faces.push_back("space3/pz.png"); // +Z
  faces.push_back("space3/nz.png"); // -Z

  skybox = new Skybox(glw, faces);

  // create premade meshes
  Mesh cubeMesh = createCube();
  Mesh sphereMesh = createSphere();
  Mesh torusMesh = createTorus();

  // load textures
  GLuint crateTex = gTextureManager.loadTexture("crate.png");
  GLuint donutTex = gTextureManager.loadTexture("donut3.jpg");
  GLuint earthTex = gTextureManager.loadTexture("planets/earth_diffuse.jpg");
  GLuint mercuryTex =
      gTextureManager.loadTexture("planets/mercury_diffuse.jpg");
  GLuint venusTex = gTextureManager.loadTexture("planets/venus_diffuse.png");
  GLuint marsTex = gTextureManager.loadTexture("planets/mars_diffuse.jpg");
  GLuint jupiterTex =
      gTextureManager.loadTexture("planets/jupiter_diffuse.jpg");
  GLuint saturnTex = gTextureManager.loadTexture("planets/saturn_diffuse.jpg");
  GLuint uranusTex = gTextureManager.loadTexture("planets/uranus_diffuse.jpg");
  GLuint neptuneTex =
      gTextureManager.loadTexture("planets/neptune_diffuse.jpg");
  GLuint plutoTex = gTextureManager.loadTexture("planets/pluto_diffuse.jpg");

  GLuint moonTex = gTextureManager.loadTexture("planets/moon_diffuse.jpg");
  GLuint sunTex = gTextureManager.loadTexture("planets/sun_diffuse.jpg");

  // create scene objects
  // auto cube1 = scene.createObject("Cube1", cubeMesh);
  // cube1->transform.position = vec3(0.0f, 0.0f, -2.0f);
  // cube1->transform.scale = vec3(0.5f, 0.3f, 0.5f);

  // auto torus1 = scene.createObject("Torus1", torusMesh);
  // torus1->transform.position = vec3(2.0f, 1.0f, -4.0f);
  // torus1->transform.scale = vec3(0.5f);
  // torus1->textureId = donutTex;

  float sizeScale = 5.0f;
  float positionScale = 10.0f;

  auto sun = scene.createObject("sun", sphereMesh);
  sun->transform.position = vec3(0.0f, 0.0f, 0.0f) * positionScale;
  sun->transform.scale = vec3(5.0f) * sizeScale;
  sun->textureId = sunTex;

  auto mercury = scene.createObject("mercury", sphereMesh);
  mercury->transform.position = vec3(6.0f, 0.0f, 0.0f) * positionScale;
  mercury->transform.scale = vec3(0.2f) * sizeScale;
  mercury->textureId = mercuryTex;

  auto venus = scene.createObject("venus", sphereMesh);
  venus->transform.position = vec3(10.0f, 0.0f, 0.0f) * positionScale;
  venus->transform.scale = vec3(0.4f) * sizeScale;
  venus->textureId = venusTex;

  auto earth = scene.createObject("earth", sphereMesh);
  earth->transform.position = vec3(14.0f, 0.0f, 0.0f) * positionScale;
  earth->transform.scale = vec3(0.45f) * sizeScale;
  earth->textureId = earthTex;

  auto mars = scene.createObject("mars", sphereMesh);
  mars->transform.position = vec3(18.0f, 0.0f, 0.0f) * positionScale;
  mars->transform.scale = vec3(0.3f) * sizeScale;
  mars->textureId = marsTex;

  auto jupiter = scene.createObject("jupiter", sphereMesh);
  jupiter->transform.position = vec3(35.0f, 0.0f, 0.0f) * positionScale;
  jupiter->transform.scale = vec3(2.5f) * sizeScale;
  jupiter->textureId = jupiterTex;

  auto saturn = scene.createObject("saturn", sphereMesh);
  saturn->transform.position = vec3(55.0f, 0.0f, 0.0f) * positionScale;
  saturn->transform.scale = vec3(2.2f) * sizeScale;
  saturn->textureId = saturnTex;

  auto uranus = scene.createObject("uranus", sphereMesh);
  uranus->transform.position = vec3(75.0f, 0.0f, 0.0f) * positionScale;
  uranus->transform.scale = vec3(1.0f) * sizeScale;
  uranus->textureId = uranusTex;

  auto neptune = scene.createObject("neptune", sphereMesh);
  neptune->transform.position = vec3(95.0f, 0.0f, 0.0f) * positionScale;
  neptune->transform.scale = vec3(1.0f) * sizeScale;
  neptune->textureId = neptuneTex;

  auto pluto = scene.createObject("pluto", sphereMesh);
  pluto->transform.position = vec3(110.0f, 0.0f, 0.0f) * positionScale;
  pluto->transform.scale = vec3(0.1f) * sizeScale;
  pluto->textureId = plutoTex;

  auto moon = scene.createObject("moon", sphereMesh);
  moon->transform.position = vec3(14.8f, 0.0f, 0.0f) * positionScale;
  moon->transform.scale = vec3(0.08f) * sizeScale;
  moon->textureId = moonTex;

  // auto car =
  //    ObjectLoader::loadOBJObject("Car.obj", vec4(0.0f, 0.0f, 0.0f, 1.0f));
  // scene.addObject(car);
  // car->transform.position = vec3(10.0f, 0.0f, -10.0f);
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
