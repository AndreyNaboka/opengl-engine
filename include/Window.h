#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
  Window(const int width, const int height, const std::string &title);
  ~Window();

  bool ShouldClose() const;
  void PollEvents() const;
  void SwapBuffers() const;
  float GetWidth() const;
  float GetHeight() const;
  GLFWwindow *GetNativeHanle() const { return _wnd; }

private:
  GLFWwindow *_wnd = nullptr;
};
