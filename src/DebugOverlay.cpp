#include "DebugOverlay.h"
#include "Camera.h"
#include "Renderer.h"
#include "Window.h"
#include "Utils/MemoryTracker.h"
#include <cstdarg>
#include <cstdio>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace {
float BytesToMiB(std::size_t bytes) {
  return static_cast<float>(bytes) / (1024.0f * 1024.0f);
}

void DebugText(const char *fmt, ...) {
  char buffer[512];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  const ImVec2 pos = ImGui::GetCursorScreenPos();
  ImGui::GetWindowDrawList()->AddText(ImVec2(pos.x + 1.0f, pos.y + 1.0f),
                                      IM_COL32(0, 0, 0, 190), buffer);
  ImGui::TextUnformatted(buffer);
}
} // namespace

bool DebugOverlay::Init(const Window &window) {
  if (_initialized)
    return true;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &imguiIO = ImGui::GetIO();
  imguiIO.FontGlobalScale = 1.0f;
  imguiIO.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

  ImFontConfig fontConfig;
  fontConfig.OversampleH = 4;
  fontConfig.OversampleV = 4;
  fontConfig.PixelSnapH = true;
  imguiIO.Fonts->Clear();
  imguiIO.Fonts->AddFontDefault(&fontConfig);

  if (!ImGui_ImplGlfw_InitForOpenGL(window.GetNativeHandle(), true)) {
    ImGui::DestroyContext();
    return false;
  }
  if (!ImGui_ImplOpenGL3_Init("#version 330 core")) {
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return false;
  }

  _initialized = true;
  return true;
}

void DebugOverlay::Render(const Camera &camera, bool freeCameraMode) const {
  if (!_initialized)
    return;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSizeConstraints(ImVec2(260.0f, 0.0f),
                                      ImVec2(420.0f, 500.0f));

  constexpr ImGuiWindowFlags debugWindowFlags =
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.02f, 0.03f, 0.82f));
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.96f, 1.0f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.65f));

  ImGui::Begin("Debug info", nullptr, debugWindowFlags);
  const ImGuiIO &io = ImGui::GetIO();
  DebugText("FPS: %.1f", io.Framerate);
  DebugText("Frame time: %.2f ms", io.DeltaTime * 1000.0f);
  DebugText("Camera mode: %s", freeCameraMode ? "Free flight" : "Physics");
  DebugText("%s", camera.GetDebugStringPos().c_str());

  const MemoryStats memoryStats = MemoryTracker::GetStats();
  DebugText("App heap: %.2f MiB", BytesToMiB(memoryStats.currentBytes));
  DebugText("Heap peak: %.2f MiB", BytesToMiB(memoryStats.peakBytes));

  const RendererStats &stats = Renderer::GetStats();
  ImGui::Separator();
  DebugText("Commands: %u", stats.commandsSubmitted);
  DebugText("Draw calls: %u", stats.drawCalls);
  DebugText("Triangles: %u", stats.triangles);
  DebugText("Shader binds: %u", stats.shaderBinds);
  DebugText("Texture binds: %u", stats.textureBinds);
  DebugText("State changes: %u", stats.stateChanges);
  DebugText("Uniform updates: %u", stats.uniformUpdates);
  DebugText("Camera uploads: %u", stats.cameraUploads);
  DebugText("Bone uploads: %u", stats.boneUploads);
  ImGui::End();

  ImGui::PopStyleColor(3);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugOverlay::Shutdown() {
  if (!_initialized)
    return;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  _initialized = false;
}
