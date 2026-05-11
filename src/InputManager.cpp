#include "InputManager.h"
#include "Logger.h"
#include <GLFW/glfw3.h>

void InputManager::Init(GLFWwindow *wnd) {
  if (!wnd) {
    LogInfo("[InputManager] Init. GLFWwindow must be not null");
    return;
  }
  _wnd = wnd;
  glfwSetWindowUserPointer(_wnd, this);
  glfwSetCursorPosCallback(_wnd, MouseCallback);
  glfwSetInputMode(_wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwGetCursorPos(_wnd, &_lastMouseX, &_lastMouseY);
}

void InputManager::Update() {
  _mouseDeltaX = 0.0f;
  _mouseDeltaY = 0.0f;
}

bool InputManager::IsKeyPressed(const int key) const {
  return glfwGetKey(_wnd, key) == GLFW_PRESS;
}

void InputManager::GetMouseDelta(float &x, float &y) const {
  x = _mouseDeltaX;
  y = _mouseDeltaY;
}

void InputManager::MouseCallback(GLFWwindow *w, double xPos, double yPos) {
  auto *input = reinterpret_cast<InputManager *>(glfwGetWindowUserPointer(w));
  if (!input)
    return;

  input->_mouseDeltaX = static_cast<float>(xPos - input->_lastMouseX);
  input->_mouseDeltaY = static_cast<float>(input->_lastMouseY - yPos);
  input->_lastMouseX = xPos;
  input->_lastMouseY = yPos;
}
