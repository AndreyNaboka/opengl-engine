#pragma once
#include <unordered_map>
#include <functional>

struct GLFWwindow;

class InputManager
{
public:
    enum class InputAction
    {
        MOVE_FORWARD,
        MOVE_BACKWARD,
        MOVE_LEFT,
        MOVE_RIGHT,
        QUIT,
        CAMERA_ROTATE,
        CAMERA_ZOOM,
        INTERACT // LBM
    };

    void Update();
    bool IsActionActive(InputAction act) const;
    bool WasActionTriggered(InputAction act) const;

    void OnKeyEvent(int key, int keyAction, int mods);
    void OnMouseButton(int button, int action, int mods);
    void OnMouseMove(double xPos, double yPos);
    void OnMouseScroll(double xOffset, double yOffset);

    struct MouseState
    {
        double DeltaX = 0.0;
        double DeltaY = 0.0;
        double ScrollY = 0.0;
    };
    const MouseState &GetMouseState() const { return _currentMouseState; }

    void BindKey(int glfwKey, InputAction act);
    void BindMouseButton(int glfwButton, InputAction act);

private:
    MouseState _currentMouseState;

    double _lastMouseX = 0.0;
    double _lastMouseY = 0.0;
    bool _isFirstMouseAction = true;

    std::unordered_map<int, InputAction> _keyBindings;
    std::unordered_map<InputAction, bool> _activeActions;
    std::unordered_map<InputAction, bool> _triggeredActions;
    std::unordered_map<int, InputAction> _mouseButtonBindings;
};