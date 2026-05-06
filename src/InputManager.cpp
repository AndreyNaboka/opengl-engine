#include "InputManager.h"
#include "Logger.h"
#include <GLFW/glfw3.h>

void InputManager::Init(GLFWwindow *wnd) {
  if (!wnd) {
    log("[InputManager] Init. GLFWwindow must be not null");
    return;
  }
  _wnd = wnd;
  glfwSetWindowUserPointer(_wnd, this);
  glfwSetCursorPosCallback(_wnd, MouseCallback);
}

void InputManager::Update() {
	_mouseDeltaX=0.0f;
	_mouseDeltaY=0.0f;
}

bool InputManager::IsKeyPressed(const int key) const { 
	return glfwGetKey(_wnd, key);
}

void InputManager::GetMouseDelta(float &x, float &y) const {}

void InputManager::MouseCallback(GLFWwindow *w, double xPos, double yPos) {}
