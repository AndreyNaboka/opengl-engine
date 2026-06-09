#include "Shader.h"
#include "TerrainGenerator.h"
#include "Window.h"
#include "InputManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "GltfLoader.h"
#include "Animator.h"
#include "CubeMapTexture.h"
#include "Sky.h"
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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

  // Ground
  auto shader = std::make_unique<Shader>("assets/shaders/terrain.vert",
                                         "assets/shaders/common.frag");
  auto texture = std::make_unique<Texture>("assets/textures/grass.png");
  auto terrain = GenerateGrid(200.0f, 200.0f, 60, 60, 15.0f);

  RenderCommand groundCmd;
  groundCmd.mesh = terrain.get();
  groundCmd.shader = shader.get();
  groundCmd.texture = texture.get();
  groundCmd.slot = 0;
  groundCmd.cullFace = false;
  groundCmd.model = glm::mat4(1.0f);

  float lastTime = 0.0f;

  // Model
  auto modelData = GltfLoader::Load("assets/models/enemy.glb");
  auto modelShader = std::make_unique<Shader>("assets/shaders/skinned.vert",
                                              "assets/shaders/common.frag");
  Animator animator;
  if (!modelData.animations.empty()) {
    animator.SetAnimation(
        modelData.animations[modelData.animations.size() - 1].get(),
        modelData.inverseBindMatrices, modelData.boneParents,
        modelData.boneNames, modelData.boneBindTranslations,
        modelData.boneBindRotations, modelData.boneBindScales,
        modelData.boneRootParentTransforms,
        true); // loop
  }
  RenderCommand cmd1;
  cmd1.mesh = modelData.mesh.get();
  cmd1.shader = modelShader.get();
  cmd1.animator = animator.IsPlaying() ? &animator : nullptr;
  if (modelData.defaultMaterialIndex >= 0 &&
      modelData.defaultMaterialIndex < modelData.materials.size()) {
    cmd1.texture =
        modelData.materials[modelData.defaultMaterialIndex].albedoTexture.get();
    cmd1.slot = 0;
  }
  glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(6.0f));
  glm::mat4 translate =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -5.0f));
  cmd1.model =
      translate * scale; // Scale применяется первым к вершинам, потом translate

  // Skybox
  const std::array<std::string, 6> skyboxFaces = {
      "assets/textures/skybox/space_rt.png", // +X
      "assets/textures/skybox/space_lf.png", // -X
      "assets/textures/skybox/space_up.png", // +Y
      "assets/textures/skybox/space_dn.png", // -Y
      "assets/textures/skybox/space_bk.png", // +Z
      "assets/textures/skybox/space_ft.png"  // -Z
  };
  auto skyboxTexture = std::make_shared<CubeMapTexture>(skyboxFaces);
  auto skyboxShader = std::make_shared<Shader>("assets/shaders/skybox.vert",
                                               "assets/shaders/skybox.frag");
  auto skyboxMesh = Sky::CreateSkyBoxMesh();

  // Настройка команды Skybox
  RenderCommand skyboxCmd;
  skyboxCmd.mesh = skyboxMesh.get();
  skyboxCmd.shader = skyboxShader.get();
  skyboxCmd.depthWrite = false; // не пишем в глубину
  skyboxCmd.depthTest = true;   // но тестируем (чтобы объекты перекрывали небо)
  skyboxCmd.cullFace = false;   // рисуем внутреннюю сторону куба
  skyboxCmd.depthFunc = DepthFunc::LessEqual;
  skyboxCmd.model = glm::mat4(1.0f);

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

    animator.Update(dt);

    skyboxShader->Bind();
    skyboxShader->SetUniformInt("u_Skybox", 0);
    skyboxTexture->Bind(0);
    // также можно передать время
    //    skyboxShader->SetUniformFloat("u_Time", glfwGetTime());

    Renderer::BeginScene(camera);
    Renderer::Submit(skyboxCmd);
    Renderer::Submit(groundCmd);
    Renderer::Submit(cmd1);
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
