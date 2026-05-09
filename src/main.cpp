#include "Window.h"
#include <GLFW/glfw3.h>
#include "InputManager.h"
#include <string>

int main() {
  Window wnd(600, 400, std::string("World"));
  InputManager input;
  input.Init(wnd.GetNativeHanle());
  wnd.PollEvents();

  while (!wnd.ShouldClose()) {
    input.Update();

    if (input.IsKeyPressed(GLFW_KEY_ESCAPE)) {
      wnd.Close();
      continue;
    }

    wnd.SwapBuffers();
    wnd.PollEvents();
  }

  return 0;
}
