#include "Window.h"
#include <memory>
#include <string>

int main() {
  std::unique_ptr<Window> wnd =
      std::make_unique<Window>(600, 400, std::string("Title"));

  wnd->Run();

  return 0;
}
