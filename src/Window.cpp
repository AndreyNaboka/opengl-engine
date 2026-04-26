#include "Logger.h"
#include <Window.h>
#include <cassert>

Window::Window(int width, int height, const std::string &title) {
  assert(width != 0);
  assert(height != 0);

  if (!glfwInit())
    log("Failed init glfw");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef PLATFORM_MACOS
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  m_wnd = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

  if (!m_wnd)
    log("Failed create window");

  glfwSetWindowUserPointer(m_wnd, this);

  glfwSetKeyCallback(m_wnd, StaticKeyCallback);
  glfwSetCursorPosCallback(m_wnd, StaticMouseCallback);

  glfwMakeContextCurrent(m_wnd);

  if (!gladLoadGL(glfwGetProcAddress))
    log("Failed to init GLAD");

  log("Create window " + title + " success");
}

Window::~Window() {
  glfwDestroyWindow(m_wnd);
  glfwTerminate();
}

void Window::Run() {
  while (!glfwWindowShouldClose(m_wnd)) {
    glfwSwapBuffers(m_wnd);
    glfwPollEvents();
  }
}

void Window::StaticKeyCallback(GLFWwindow *w, int key, int scancode, int action,
                               int mods) {
  Window *window = static_cast<Window *>(glfwGetWindowUserPointer(w));
  window->ProcessKeyInput(key, action);
}

void Window::StaticMouseCallback(GLFWwindow *w, double xpos, double ypos) {
  Window *window = static_cast<Window *>(glfwGetWindowUserPointer(w));
  window->ProcessMouse(xpos, ypos);
}

void Window::ProcessKeyInput(int key, int action) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      Logger::GetInstance().out("Close window and exit");
      glfwSetWindowShouldClose(m_wnd, 1);
    }
  }
}

bool Window::IsActive() const {
	return true;
}

void Window::ProcessMouse(double xpos, double ypos) {
  // log("Mouse: ", {xpos, ypos});
}
