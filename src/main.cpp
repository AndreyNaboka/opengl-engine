#include "Shader.h"
#include "TerrainGenerator.h"
#include "Window.h"
#include "InputManager.h"
#include <Camera.h>
#include <GLFW/glfw3.h>
#include <Renderer.h>
#include <memory>
#include <string>

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

  wnd.PollEvents();

  while (!wnd.ShouldClose()) {
    float now = glfwGetTime();
    float dt = now - lastTime;
    lastTime = now;

    input.Update();
    camera.Update(input, dt);

    if (input.IsKeyPressed(GLFW_KEY_ESCAPE)) {
      wnd.Close();
      continue;
    }

    Renderer::BeginScene(camera);
    Renderer::Submit(groundCmd);
    Renderer::EndScene();

    wnd.SwapBuffers();
    wnd.PollEvents();
  }

  return 0;
}
