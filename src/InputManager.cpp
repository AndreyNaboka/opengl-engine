#include "InputManager.h"
#include "Utils/Logger.h"
#include <GLFW/glfw3.h>

void InputManager::Init(GLFWwindow *wnd) {
  if (!wnd) {
    LogInfo("[InputManager] Init. GLFWwindow must be not null");
    return;
  }
  _wnd = wnd;
  glfwSetWindowUserPointer(_wnd, this);
  glfwSetCursorPosCallback(_wnd, MouseCallback);
  SetCursorEnabled(false);
  glfwGetCursorPos(_wnd, &_lastMouseX, &_lastMouseY);
  for (int key = 0; key <= GLFW_KEY_LAST; ++key) {
    _previousKeyStates[key] = glfwGetKey(_wnd, key) == GLFW_PRESS;
  }
}

void InputManager::Update() {
  _mouseDeltaX = 0.0f;
  _mouseDeltaY = 0.0f;

  if (!_wnd)
    return;

  for (int key = 0; key <= GLFW_KEY_LAST; ++key) {
    _previousKeyStates[key] = glfwGetKey(_wnd, key) == GLFW_PRESS;
  }
}

bool InputManager::IsKeyPressed(const int key) const {
  if (!_wnd || !IsValidKey(key))
    return false;
  return glfwGetKey(_wnd, key) == GLFW_PRESS;
}

bool InputManager::IsKeyJustPressed(const int key) const {
  if (!_wnd || !IsValidKey(key))
    return false;

  const bool pressed = glfwGetKey(_wnd, key) == GLFW_PRESS;
  return pressed && !_previousKeyStates[key];
}

void InputManager::GetMouseDelta(float &x, float &y) const {
  x = _mouseDeltaX;
  y = _mouseDeltaY;
}

void InputManager::SetCursorEnabled(bool enabled) {
  if (!_wnd)
    return;

  _cursorEnabled = enabled;
  glfwSetInputMode(_wnd, GLFW_CURSOR,
                   enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
  glfwGetCursorPos(_wnd, &_lastMouseX, &_lastMouseY);
  _mouseDeltaX = 0.0f;
  _mouseDeltaY = 0.0f;
}

bool InputManager::IsValidKey(int key) const {
  return key >= 0 && key <= GLFW_KEY_LAST;
}

void InputManager::MouseCallback(GLFWwindow *w, double xPos, double yPos) {
  auto *input = reinterpret_cast<InputManager *>(glfwGetWindowUserPointer(w));
  if (!input)
    return;

  if (input->_cursorEnabled) {
    input->_mouseDeltaX = 0.0f;
    input->_mouseDeltaY = 0.0f;
    input->_lastMouseX = xPos;
    input->_lastMouseY = yPos;
    return;
  }

  input->_mouseDeltaX = static_cast<float>(xPos - input->_lastMouseX);
  input->_mouseDeltaY = static_cast<float>(input->_lastMouseY - yPos);
  input->_lastMouseX = xPos;
  input->_lastMouseY = yPos;
}
