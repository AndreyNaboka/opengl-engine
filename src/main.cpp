#include "Shader.h"
#include "TerrainGenerator.h"
#include "Window.h"
#include "InputManager.h"
#include <Camera.h>
#include <GLFW/glfw3.h>
#include <Renderer.h>
#include <memory>
#include <string>
#include "GltfLoader.h"
#include "Animator.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void RenderDebugText(const Camera &cam) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(250, 0), ImGuiCond_FirstUseEver);

  ImGui::Begin("Debug info");
  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
  ImGui::TextWrapped("%s", cam.GetDebugStringPos().c_str());
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

  // Ground
  auto shader = std::make_unique<Shader>("assets/shaders/terrain.vert",
                                         "assets/shaders/terrain.frag");
  auto texture = std::make_unique<Texture>("assets/textures/grass.png");
  auto terrain = GenerateGrid(200.0f, 200.0f, 60, 60, 15.0f);

  RenderCommand groundCmd;
  groundCmd.mesh = terrain.get();
  groundCmd.shader = shader.get();
  groundCmd.texture = texture.get();
  groundCmd.slot = 0;
  groundCmd.model = glm::mat4(1.0f);

  float lastTime = 0.0f;

  // Model
  auto modelData = GltfLoader::Load("assets/models/enemy.glb");
  auto modelShader = std::make_unique<Shader>("assets/shaders/skinned.vert",
                                              "assets/shaders/terrain.frag");
  Animator animator;
  if (!modelData.animations.empty()) {
    animator.SetAnimation(modelData.animations[0].get(),
                          modelData.inverseBindMatrices, true);
  }
  RenderCommand cmd1;
  cmd1.mesh = modelData.mesh.get();
  cmd1.shader = modelShader.get();
  cmd1.animator = &animator;
  cmd1.model = glm::translate(glm::mat4(1.0f), {-2.0f, 0.0f, 0.0f});

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

    int fbW = wnd.GetFramebufferWidth();
    int fbH = wnd.GetFramebufferHeight();
    glViewport(0, 0, fbW, fbH);
    camera.SetAspect(static_cast<float>(fbW) / static_cast<float>(fbH));

    animator.Update(dt);

    Renderer::BeginScene(camera);
    Renderer::Submit(cmd1);
    Renderer::Submit(groundCmd);
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
