#include "Logger.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <Window.h>

Window::Window(const int width, const int height, const std::string &title) {
  if (!glfwInit())
    LogInfo("[Window] Can't init glfw");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _wnd = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!_wnd)
    LogInfo("[Window] Can't create window");

  glfwMakeContextCurrent(_wnd);

  const int versionGLAD = gladLoadGL(glfwGetProcAddress);
  if (versionGLAD == 0) {
    LogInfo("[Window] Failed to load OpenGL context");
  }

  glfwSetFramebufferSizeCallback(
      _wnd, [](GLFWwindow *, int w, int h) { glViewport(0, 0, w, h); });
  glfwSetInputMode(_wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Window::~Window() {
  glfwDestroyWindow(_wnd);
  glfwTerminate();
}

void Window::Close() const { glfwSetWindowShouldClose(_wnd, GLFW_TRUE); }

bool Window::ShouldClose() const { return glfwWindowShouldClose(_wnd); }

void Window::PollEvents() const { glfwPollEvents(); }

void Window::SwapBuffers() const { glfwSwapBuffers(_wnd); }

float Window::GetWidth() const {
  int w, h;
  glfwGetWindowSize(_wnd, &w, &h);
  return static_cast<float>(w);
}

float Window::GetHeight() const {
  int w, h;
  glfwGetWindowSize(_wnd, &w, &h);
  return static_cast<float>(h);
}
