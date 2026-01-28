#pragma once
#include <unordered_map>
#include <functional>

struct GLFWwindow;

class input_manager
{
public:
    enum class input_action
    {
        MOVE_FORWARD,
        MOVE_BACKWARD,
        MOVE_LEFT,
        MOVE_RIGHT,
        QUIT
    };

    void on_key_event(int key, int key_action, int mods);
    void update();
    bool is_action_active(input_action act) const;
    bool was_action_triggered(input_action act) const;
    void bind_key(int glfw_key, input_action act);

private:
    std::unordered_map<int, input_action> _key_bindings;
    std::unordered_map<input_action, bool> _active_actions;
    std::unordered_map<input_action, bool> _triggered_actions;
};