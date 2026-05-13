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

int main() {
  Window wnd(1200, 800, std::string("World"));
  InputManager input;
  input.Init(wnd.GetNativeHanle());

  Renderer::Init();
  Camera camera({0.0f, 8.0f, 25.0f}, 60.0f, wnd.GetWidth() / wnd.GetHeight());

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
  
  GltfLoader::Load("assets/models/enemy.glb");

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

    Renderer::BeginScene(camera);
    Renderer::Submit(groundCmd);
    Renderer::EndScene();

    wnd.SwapBuffers();

    input.Update();
  }

  return 0;
}
