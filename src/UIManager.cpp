#include "../include/UIManager.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_glfw.h"
#include "../external/imgui/imgui_impl_opengl3.h"

void UIManager::init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 420");
}

void UIManager::beginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void UIManager::renderFPS(float fps, bool &showControls) {
  ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
  ImGui::Begin("FPS", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBackground);

  ImGui::SetWindowFontScale(1.5f);
  ImGui::Text("FPS: %d", int(fps));

  if (!showControls) {
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
                       "Press H to show controls");
  }

  ImGui::End();
}

void UIManager::renderControls(bool &showControls) {
  if (!showControls)
    return;

  ImGui::SetNextWindowPos(ImVec2(10, 100), ImGuiCond_Always);
  ImGui::Begin("Controls", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove);

  ImGui::SetWindowFontScale(1.5f);

  ImGui::Text("Camera Controls:");
  ImGui::Text("WASD: move");
  ImGui::Text("Space / LShift: up/down");
  ImGui::Text("Arrow keys: look");

  ImGui::Dummy(ImVec2(0, 10));

  ImGui::Text("Crate Controls:");
  ImGui::Text("U/O/P: rotate");
  ImGui::Text("I/K/J/L/N/M: move");

  ImGui::Dummy(ImVec2(0, 10));
  ImGui::Text("Press H to hide");

  ImGui::End();
}

void UIManager::renderCameraInfo(const glm::vec3 &cameraPosition,
                                 float &cameraYaw, float &cameraPitch) {
  ImVec2 displaySize = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(displaySize.x - 10, 10), ImGuiCond_Always,
                          ImVec2(1.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("FPS", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBackground);
  ImGui::PopStyleVar();
  ImGui::SetWindowFontScale(1.5f);
  ImGui::Text("X, Y, Z: %.1f, %.1f, %.1f", cameraPosition.x, cameraPosition.y,
              cameraPosition.z);
  ImGui::Text("Camera: %.1f, %.1f", cameraYaw, cameraPitch);
  ImGui::End();
}

void UIManager::endFrame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
