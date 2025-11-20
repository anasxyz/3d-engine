#define _USE_MATH_DEFINES // For M_PI on some platforms
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random> // Include for better random generation, though rand() is simpler

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
float renderDistance = 10000.0f;

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

// solar system specific state and constants
// scaling factors
const float sizeScale = 5.0f;
const float posScale = 10.0f;

// orbit radii (pre calculated from init setup position.x * posScale)
const float mercuryRadius = 6.0f * posScale;
const float venusRadius = 10.0f * posScale;
const float earthRadius = 14.0f * posScale;
const float marsRadius = 18.0f * posScale;
const float jupiterRadius = 35.0f * posScale;
const float saturnRadius = 55.0f * posScale;
const float uranusRadius = 75.0f * posScale;
const float neptuneRadius = 95.0f * posScale;
const float plutoRadius = 110.0f * posScale;
const float moonRadius = 4.0f * sizeScale;

// orbital speeds (degrees per second)
const float mercuryOrbitSpeed = 5.0f;
const float venusOrbitSpeed = 3.75f;
const float earthOrbitSpeed = 2.5f;
const float marsOrbitSpeed = 2.0f;
const float jupiterOrbitSpeed = 1.0f;
const float saturnOrbitSpeed = 0.75f;
const float uranusOrbitSpeed = 0.375f;
const float neptuneOrbitSpeed = 0.25f;
const float plutoOrbitSpeed = 0.125f;
const float moonOrbitSpeed = 15.0f;

// axial rotation speeds (degrees per second)
// how fast the planet spins
const float sunAxialSpeed = 0.50f;
const float mercuryAxialSpeed = 0.375f;
const float venusAxialSpeed = 0.25f;
const float earthAxialSpeed = 7.5f;
const float marsAxialSpeed = 0.45f;
const float jupiterAxialSpeed = 12.5f;
const float saturnAxialSpeed = 11.25f;
const float uranusAxialSpeed = 0.3f;
const float neptuneAxialSpeed = 2.75f;
const float plutoAxialSpeed = 0.125f;
const float moonAxialSpeed = 0.50f;

// current orbital angles (in radians)
// state variables updated every frame
float mercuryOrbitAngle = 0.0f;
float venusOrbitAngle = 0.0f;
float earthOrbitAngle = 0.0f;
float marsOrbitAngle = 0.0f;
float jupiterOrbitAngle = 0.0f;
float saturnOrbitAngle = 0.0f;
float uranusOrbitAngle = 0.0f;
float neptuneOrbitAngle = 0.0f;
float plutoOrbitAngle = 0.0f;
float moonOrbitAngle = 0.0f;

// set the initial position of a planet based on its radius and starting angle
void setPlanetInitialPosition(Object *obj, float radius, float angleInRadians) {
  obj->transform.position.x = radius * glm::cos(angleInRadians);
  obj->transform.position.z = radius * glm::sin(angleInRadians);
  obj->transform.position.y = 0.0f;
}

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
    obj.transform.position.y += 100.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    obj.transform.position.x -= 100.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    obj.transform.position.y -= 100.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    obj.transform.position.x += 100.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    obj.transform.position.z += 100.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    obj.transform.position.z -= 100.0f * deltaTime;
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
      glm::perspective(glm::radians(45.0f), aspect, 0.1f, renderDistance);

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

  // find earth object to correctly calculate moon's orbit center
  Object *earthObj = nullptr;
  for (auto &obj : scene.objects) {
    if (obj->name == "earth") {
      earthObj = obj.get();
      break;
    }
  }

  // separate the movement for the moon to ensure earths position is updated
  // first bc scene.objects is a vector and we can rely on the order of
  // insertion which is the earth then the moon

  for (auto &obj : scene.objects) {

    // movement and orbital logic
    float orbitSpeed = 0.0f;
    float axialSpeed = 0.0f;
    float radius = 0.0f;
    float *anglePtr = nullptr;
    vec3 orbitCenter = vec3(0.0f);

    if (obj->name == "sun") {
      // sun only rotates on its axis
      axialSpeed = sunAxialSpeed;
      obj->transform.rotation.y += glm::radians(axialSpeed * deltaTime);
    }
    // planets (orbit the sun)
    else if (obj->name == "mercury") {
      orbitSpeed = mercuryOrbitSpeed;
      axialSpeed = mercuryAxialSpeed;
      radius = mercuryRadius;
      anglePtr = &mercuryOrbitAngle;
    } else if (obj->name == "venus") {
      orbitSpeed = venusOrbitSpeed;
      axialSpeed = venusAxialSpeed;
      radius = venusRadius;
      anglePtr = &venusOrbitAngle;
    } else if (obj->name == "earth") {
      orbitSpeed = earthOrbitSpeed;
      axialSpeed = earthAxialSpeed;
      radius = earthRadius;
      anglePtr = &earthOrbitAngle;
      obj->transform.rotation.x = glm::radians(23.5f);
    } else if (obj->name == "mars") {
      orbitSpeed = marsOrbitSpeed;
      axialSpeed = marsAxialSpeed;
      radius = marsRadius;
      anglePtr = &marsOrbitAngle;
    } else if (obj->name == "jupiter") {
      orbitSpeed = jupiterOrbitSpeed;
      axialSpeed = jupiterAxialSpeed;
      radius = jupiterRadius;
      anglePtr = &jupiterOrbitAngle;
    } else if (obj->name == "saturn") {
      orbitSpeed = saturnOrbitSpeed;
      axialSpeed = saturnAxialSpeed;
      radius = saturnRadius;
      anglePtr = &saturnOrbitAngle;
    } else if (obj->name == "uranus") {
      orbitSpeed = uranusOrbitSpeed;
      axialSpeed = uranusAxialSpeed;
      radius = uranusRadius;
      anglePtr = &uranusOrbitAngle;
    } else if (obj->name == "neptune") {
      orbitSpeed = neptuneOrbitSpeed;
      axialSpeed = neptuneAxialSpeed;
      radius = neptuneRadius;
      anglePtr = &neptuneOrbitAngle;
    } else if (obj->name == "pluto") {
      orbitSpeed = plutoOrbitSpeed;
      axialSpeed = plutoAxialSpeed;
      radius = plutoRadius;
      anglePtr = &plutoOrbitAngle;
    }
    // moon special case (orbit the earth)
    else if (obj->name == "moon" && earthObj != nullptr) {
      orbitSpeed = moonOrbitSpeed;
      axialSpeed = moonAxialSpeed;
      radius = moonRadius;
      anglePtr = &moonOrbitAngle;
      // the moon's center of orbit is the earth's current position
      orbitCenter = earthObj->transform.position;
    }

    // apply axial rotation (spin)
    if (axialSpeed > 0.0f) {
      obj->transform.rotation.y += glm::radians(axialSpeed * deltaTime);
    }

    // apply orbital translation (for everything that orbits)
    if (anglePtr != nullptr && orbitSpeed > 0.0f) {
      *anglePtr += glm::radians(orbitSpeed * deltaTime);
      if (*anglePtr > 2.0f * M_PI) {
        *anglePtr -= 2.0f * M_PI;
      }

      obj->transform.position.x = orbitCenter.x + radius * glm::cos(*anglePtr);
      obj->transform.position.z = orbitCenter.z + radius * glm::sin(*anglePtr);
    }

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
  GLuint venusTex = gTextureManager.loadTexture("planets/venus_diffuse.jpg");
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

  // seed random number generator to start planets in different places
  std::srand(static_cast<unsigned int>(time(nullptr)));

  // assign random starting orbital angles (0 to 360 degrees, converted to
  // radians)
  auto randAngle = []() { return glm::radians((float)(std::rand() % 360)); };

  mercuryOrbitAngle = randAngle();
  venusOrbitAngle = randAngle();
  earthOrbitAngle = randAngle();
  marsOrbitAngle = randAngle();
  jupiterOrbitAngle = randAngle();
  saturnOrbitAngle = randAngle();
  uranusOrbitAngle = randAngle();
  neptuneOrbitAngle = randAngle();
  plutoOrbitAngle = randAngle();
  moonOrbitAngle = randAngle();

  // create scene objects
  auto sun = scene.createObject("sun", sphereMesh);
  sun->transform.position = vec3(0.0f);
  sun->transform.scale = vec3(5.0f) * sizeScale;
  sun->textureId = sunTex;

  auto mercury = scene.createObject("mercury", sphereMesh);
  setPlanetInitialPosition(mercury.get(), mercuryRadius, mercuryOrbitAngle);
  mercury->transform.scale = vec3(0.2f) * sizeScale;
  mercury->textureId = mercuryTex;

  auto venus = scene.createObject("venus", sphereMesh);
  setPlanetInitialPosition(venus.get(), venusRadius, venusOrbitAngle);
  venus->transform.scale = vec3(0.4f) * sizeScale;
  venus->textureId = venusTex;

  auto earth = scene.createObject("earth", sphereMesh);
  setPlanetInitialPosition(earth.get(), earthRadius, earthOrbitAngle);
  earth->transform.scale = vec3(0.45f) * sizeScale;
  earth->textureId = earthTex;

  auto mars = scene.createObject("mars", sphereMesh);
  setPlanetInitialPosition(mars.get(), marsRadius, marsOrbitAngle);
  mars->transform.scale = vec3(0.3f) * sizeScale;
  mars->textureId = marsTex;

  auto jupiter = scene.createObject("jupiter", sphereMesh);
  setPlanetInitialPosition(jupiter.get(), jupiterRadius, jupiterOrbitAngle);
  jupiter->transform.scale = vec3(2.5f) * sizeScale;
  jupiter->textureId = jupiterTex;

  auto saturn = scene.createObject("saturn", sphereMesh);
  setPlanetInitialPosition(saturn.get(), saturnRadius, saturnOrbitAngle);
  saturn->transform.scale = vec3(2.2f) * sizeScale;
  saturn->textureId = saturnTex;

  auto uranus = scene.createObject("uranus", sphereMesh);
  setPlanetInitialPosition(uranus.get(), uranusRadius, uranusOrbitAngle);
  uranus->transform.scale = vec3(1.0f) * sizeScale;
  uranus->textureId = uranusTex;

  auto neptune = scene.createObject("neptune", sphereMesh);
  setPlanetInitialPosition(neptune.get(), neptuneRadius, neptuneOrbitAngle);
  neptune->transform.scale = vec3(1.0f) * sizeScale;
  neptune->textureId = neptuneTex;

  auto pluto = scene.createObject("pluto", sphereMesh);
  setPlanetInitialPosition(pluto.get(), plutoRadius, plutoOrbitAngle);
  pluto->transform.scale = vec3(0.1f) * sizeScale;
  pluto->textureId = plutoTex;

  vec3 initialEarthPos = earth->transform.position;

  auto moon = scene.createObject("moon", sphereMesh);
  moon->transform.position.x =
      initialEarthPos.x + moonRadius * glm::cos(moonOrbitAngle);
  moon->transform.position.z =
      initialEarthPos.z + moonRadius * glm::sin(moonOrbitAngle);
  moon->transform.position.y = 0.0f;
  moon->transform.scale = vec3(0.08f) * sizeScale;
  moon->textureId = moonTex;
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
