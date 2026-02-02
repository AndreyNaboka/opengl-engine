#include "input_manager.h"
#include <glfw/glfw3.h>

void input_manager::bind_key(int glfw_key, input_action act)
{
    _key_bindings[glfw_key] = act;
}

void input_manager::bind_mouse_button(int glfw_button, input_action act)
{
}

void input_manager::on_mouse_button(int button, int action, int mods)
{
}

void input_manager::on_mouse_move(double xpos, double ypos)
{
}

void input_manager::on_mouse_scroll(double xoffset, double yoffset)
{
}

void input_manager::on_key_event(int key, int key_action, int mods)
{
    const auto it = _key_bindings.find(key);
    if (it == _key_bindings.end())
        return;

    const input_action act = it->second;
    if (key_action == GLFW_PRESS)
    {
        _active_actions[act] = true;
        _triggered_actions[act] = true;
    }
    else if (key_action == GLFW_RELEASE)
    {
        _active_actions[act] = false;
    }
}

void input_manager::update()
{
    for (auto &pair : _triggered_actions)
        pair.second = false;
}

bool input_manager::is_action_active(input_action act) const
{
    const auto it = _active_actions.find(act);
    return (it != _active_actions.end()) && it->second;
}

bool input_manager::was_action_triggered(input_action act) const
{
    const auto it = _triggered_actions.find(act);
    return (it != _triggered_actions.end()) && it->second;
}
