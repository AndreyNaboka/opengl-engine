#include "Window.h"
#include "InputManager.h"
#include "Camera.h"
#include "DebugOverlay.h"
#include "Renderer.h"
#include "Level.h"
#include <algorithm>
#include <string>
#include <GLFW/glfw3.h>

int main() {
  Window wnd(1200, 800, std::string("World"));
  if (!wnd.IsValid()) {
    return 1;
  }

  InputManager input;
  input.Init(wnd.GetNativeHandle());

  Renderer::Init();
  Camera camera({0.0f, 8.0f, 25.0f}, 60.0f, wnd.GetWidth() / wnd.GetHeight());
  Level level;

  float lastTime = 0.0f;

  DebugOverlay debugOverlay;
  if (!debugOverlay.Init(wnd)) {
    Renderer::Shutdown();
    return 1;
  }

  wnd.PollEvents();

  while (!wnd.ShouldClose()) {
    float now = glfwGetTime();
    float dt = now - lastTime;
    lastTime = now;

    input.Update();
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

    debugOverlay.Render(camera, camera.IsFreeCameraMode());

    wnd.SwapBuffers();
  }

  debugOverlay.Shutdown();
  Renderer::Shutdown();

  return 0;
}
