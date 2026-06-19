#pragma once
#include <GLFW/glfw3.h>
#include <array>

class InputManager {
public:
  void Init(GLFWwindow *wnd);
  void Update();

  bool IsKeyPressed(const int key) const;
  bool IsKeyJustPressed(const int key) const;
  void GetMouseDelta(float &x, float &y) const;
  void SetCursorEnabled(bool enabled);
  bool IsCursorEnabled() const { return _cursorEnabled; }

private:
  GLFWwindow *_wnd = nullptr;
  float _mouseDeltaX = 0.0f;
  float _mouseDeltaY = 0.0f;
  double _lastMouseX = 0.0;
  double _lastMouseY = 0.0;
  bool _cursorEnabled = false;
  std::array<bool, GLFW_KEY_LAST + 1> _currentKeyStates{};
  std::array<bool, GLFW_KEY_LAST + 1> _previousKeyStates{};

private:
  bool IsValidKey(int key) const;
  static void KeyCallback(GLFWwindow *w, int key, int scancode, int action,
                          int mods);
  static void MouseCallback(GLFWwindow *w, double xPos, double yPos);
};
