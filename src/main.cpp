#include "Window.h"
#include <string>

int main() {
  std::unique_ptr<Window> wnd =
      std::make_unique<Window>(600, 400, std::string("World"));

  return 0;
}
