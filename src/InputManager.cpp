#include "InputManager.h"
#include <glfw/glfw3.h>

void InputManager::BindKey(int glfwKey, InputAction act)
{
    _keyBindings[glfwKey] = act;
}

void InputManager::BindMouseButton(int glfwButton, InputAction act)
{
    _mouseButtonBindings[glfwButton] = act;
}

void InputManager::OnMouseButton(int button, int action, int mods)
{
    auto it = _mouseButtonBindings.find(button);
    if (it == _mouseButtonBindings.end())
        return;

    InputAction act = it->second;
    if (action == GLFW_PRESS)
    {
        _activeActions[act] = true;
        _triggeredActions[act] = true;
    }
    else
    {
        _activeActions[act] = false;
    }
}

void InputManager::OnMouseMove(double xPos, double yPos)
{
    if (_isFirstMouseAction)
    {
        _lastMouseX = xPos;
        _lastMouseY = yPos;
        _isFirstMouseAction = false;
        return;
    }
    _currentMouseState.DeltaX = xPos - _lastMouseX;
    _currentMouseState.DeltaY = _lastMouseY - yPos;
    _lastMouseX = xPos;
    _lastMouseY = yPos;
}

void InputManager::OnMouseScroll(double xOffset, double yOffset)
{
    _currentMouseState.ScrollY += yOffset;
}

void InputManager::OnKeyEvent(int key, int keyAction, int mods)
{
    const auto it = _keyBindings.find(key);
    if (it == _keyBindings.end())
        return;

    const InputAction act = it->second;
    if (keyAction == GLFW_PRESS)
    {
        _activeActions[act] = true;
        _triggeredActions[act] = true;
    }
    else if (keyAction == GLFW_RELEASE)
    {
        _activeActions[act] = false;
    }
}

void InputManager::Update()
{
    for (auto &pair : _triggeredActions)
        pair.second = false;

    _currentMouseState.DeltaX = 0.0;
    _currentMouseState.DeltaY = 0.0;
    _currentMouseState.ScrollY = 0.0;
}

bool InputManager::IsActionActive(InputAction act) const
{
    const auto it = _activeActions.find(act);
    return (it != _activeActions.end()) && it->second;
}

bool InputManager::WasActionTriggered(InputAction act) const
{
    const auto it = _triggeredActions.find(act);
    return (it != _triggeredActions.end()) && it->second;
}
