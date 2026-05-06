#pragma once
#include <GLFW/glfw3.h>

class InputManager {
public:
  void Init(GLFWwindow *wnd);
  void Update();

  bool IsKeyPressed(const int key) const;
  void GetMouseDelta(float &x, float &y) const;

private:
  GLFWwindow *_wnd;
  float _mouseDeltaX = 0.0f;
  float _mouseDeltaY = 0.0f;
  double _lastMouseX = 0.0;
  double _lastMouseY = 0.0;

private:
  static void MouseCallback(GLFWwindow *w, double xPos, double yPos);
};
