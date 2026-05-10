#include "Window.h"
#include "InputManager.h"
#include <Camera.h>
#include <GLFW/glfw3.h>
#include <Renderer.h>
#include <string>

int main() {
  Window wnd(600, 400, std::string("World"));
  InputManager input;
  input.Init(wnd.GetNativeHanle());

  Renderer::Init();
  Camera camera({0.0f, 8.0f, 25.0f}, 60.0f, wnd.GetWidth() / wnd.GetHeight());

  wnd.PollEvents();

  while (!wnd.ShouldClose()) {
    input.Update();

    if (input.IsKeyPressed(GLFW_KEY_ESCAPE)) {
      wnd.Close();
      continue;
    }

    Renderer::BeginScene(camera);
    Renderer::EndScene();

    wnd.SwapBuffers();
    wnd.PollEvents();
  }

  return 0;
}
