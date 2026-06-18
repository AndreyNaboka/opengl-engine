#include "Window.h"
#include "InputManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "Level.h"
#include "MemoryTracker.h"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <GLFW/glfw3.h>
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

void RenderDebugText(const Camera &cam, bool freeCameraMode) {
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
  DebugText("%s", cam.GetDebugStringPos().c_str());
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

int main() {
  Window wnd(1200, 800, std::string("World"));
  if (!wnd.IsValid()) {
    return 1;
  }

  InputManager input;
  input.Init(wnd.GetNativeHanle());

  Renderer::Init();
  Camera camera({0.0f, 8.0f, 25.0f}, 60.0f, wnd.GetWidth() / wnd.GetHeight());
  Level level;

  float lastTime = 0.0f;

  // ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &imguiIO = ImGui::GetIO();
  imguiIO.FontGlobalScale = 1.0f;
  ImFontConfig fontConfig;
  fontConfig.OversampleH = 4;
  fontConfig.OversampleV = 4;
  fontConfig.PixelSnapH = true;
  imguiIO.Fonts->Clear();
  imguiIO.Fonts->AddFontDefault(&fontConfig);
  ImGui_ImplGlfw_InitForOpenGL(wnd.GetNativeHanle(), true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  wnd.PollEvents();

  while (!wnd.ShouldClose()) {
    float now = glfwGetTime();
    float dt = now - lastTime;
    lastTime = now;

    wnd.PollEvents();

    if (input.IsKeyJustPressed(GLFW_KEY_F1)) {
      input.SetCursorEnabled(!input.IsCursorEnabled());
    }

    if (input.IsKeyPressed(GLFW_KEY_ESCAPE)) {
      wnd.Close();
      continue;
    }

    const int fbW = std::max(1, wnd.GetFramebufferWidth());
    const int fbH = std::max(1, wnd.GetFramebufferHeight());
    glViewport(0, 0, fbW, fbH);
    camera.SetAspect(static_cast<float>(fbW) / static_cast<float>(fbH));

    level.Update(input, camera, dt);

    Renderer::BeginScene(camera);
    level.Render();
    Renderer::EndScene();

    RenderDebugText(camera, camera.IsFreeCameraMode());

    wnd.SwapBuffers();

    input.Update();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
