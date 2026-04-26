#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
  Window(int width, int height, const std::string &title);
  ~Window();
  void Run();
  bool IsActive() const;

private:
  static void StaticKeyCallback(GLFWwindow *w, int key, int scancode,
                                int action, int mods);
  static void StaticMouseCallback(GLFWwindow *w, double xpos, double ypos);

  void ProcessKeyInput(int key, int action);
  void ProcessMouse(double xpos, double ypos);

private:
  GLFWwindow *m_wnd = nullptr;
};
