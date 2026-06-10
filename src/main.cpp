#include "Window.h"
#include "InputManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "Level.h"
#include <algorithm>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void RenderDebugText(const Camera &cam) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);

  ImGui::Begin("Debug info");
  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
  ImGui::Text("%s", cam.GetDebugStringPos().c_str());
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
  Window wnd(1200, 800, std::string("World"));
  InputManager input;
  input.Init(wnd.GetNativeHanle());

  Renderer::Init();
  Camera camera({0.0f, 8.0f, 25.0f}, 60.0f, wnd.GetWidth() / wnd.GetHeight());
  Level level;

  float lastTime = 0.0f;

  // ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO().FontGlobalScale = 0.75f;
  ImGui_ImplGlfw_InitForOpenGL(wnd.GetNativeHanle(), true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  wnd.PollEvents();

  while (!wnd.ShouldClose()) {
    float now = glfwGetTime();
    float dt = now - lastTime;
    lastTime = now;

    wnd.PollEvents();

    camera.Update(input, dt);

    if (input.IsKeyPressed(GLFW_KEY_ESCAPE)) {
      wnd.Close();
      continue;
    }

    const int fbW = std::max(1, wnd.GetFramebufferWidth());
    const int fbH = std::max(1, wnd.GetFramebufferHeight());
    glViewport(0, 0, fbW, fbH);
    camera.SetAspect(static_cast<float>(fbW) / static_cast<float>(fbH));

    level.Update(dt);

    Renderer::BeginScene(camera);
    level.Render();
    Renderer::EndScene();

    RenderDebugText(camera);

    wnd.SwapBuffers();

    input.Update();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
