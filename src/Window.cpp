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

  auto GetGLString = [](GLenum name) -> std::string {
    const char *str = reinterpret_cast<const char *>(glGetString(name));
    return str ? str : "Unknown";
  };

  const std::string renderer = "GPU:" + GetGLString(GL_RENDERER);
  const std::string vendor = "Vendor: " + GetGLString(GL_VENDOR);
  const std::string version = "OpenGL: " + GetGLString(GL_VERSION);
  LogInfo(renderer);
  LogInfo(vendor);
  LogInfo(version);

  glfwSetFramebufferSizeCallback(
      _wnd, [](GLFWwindow *, int w, int h) { glViewport(0, 0, w, h); });
}

Window::~Window() {
  glfwDestroyWindow(_wnd);
  glfwTerminate();
}

int Window::GetFramebufferWidth() const {
  int w, h;
  glfwGetFramebufferSize(_wnd, &w, &h);
  return w;
}

int Window::GetFramebufferHeight() const {
  int w, h;
  glfwGetFramebufferSize(_wnd, &w, &h);
  return h;
}

float Window::GetAspectRatio() const {
  return static_cast<float>(GetFramebufferWidth()) /
         static_cast<float>(GetFramebufferHeight());
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
