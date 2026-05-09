#include "Window.h"
#include "InputManager.h"
#include <string>

int main() {
  Window wnd(600, 400, std::string("World"));
  InputManager input;
  input.Init(wnd.GetNativeHanle());
  wnd.PollEvents();

  while (!wnd.ShouldClose()) {
    input.Update();
    wnd.SwapBuffers();
    wnd.PollEvents();
  }

  return 0;
}
