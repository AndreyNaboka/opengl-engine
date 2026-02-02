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
        QUIT,
        CAMERA_ROTATE,
        CAMERA_ZOOM,
        INTERACT // LBM
    };

    void update();
    bool is_action_active(input_action act) const;
    bool was_action_triggered(input_action act) const;

    void on_key_event(int key, int key_action, int mods);
    void on_mouse_button(int button, int action, int mods);
    void on_mouse_move(double xpos, double ypos);
    void on_mouse_scroll(double xoffset, double yoffset);

    struct mouse_state
    {
        double delta_x = 0.0;
        double delta_y = 0.0;
        double scroll_y = 0.0;
    };
    const mouse_state &get_mouse_state() const { return _ms; }

    void bind_key(int glfw_key, input_action act);
    void bind_mouse_button(int glfw_button, input_action act);

private:
    mouse_state _ms;
    std::unordered_map<int, input_action> _mouse_button_bindings;
    bool _is_first_mouse_action = true;
    double _last_mouse_x = 0.0;
    double _last_mouse_y = 0.0;

    std::unordered_map<int, input_action> _key_bindings;
    std::unordered_map<input_action, bool> _active_actions;
    std::unordered_map<input_action, bool> _triggered_actions;
};