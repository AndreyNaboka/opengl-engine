#pragma once
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <functional>

#include "glad/glad.h"
#include <glfw/glfw3.h>

class window
{
public:
    using wnd_ptr = std::shared_ptr<window>;

public:
    using key_callback_proxy = std::function<void(int key, int scancode, int action, int mods)>;
    using mouse_move_callback_proxy = std::function<void(double xpos, double ypos)>;
    using scroll_callback_proxy = std::function<void(double xoffset, double yoffset)>;
    using mouse_button_callback_proxy = std::function<void(int button, int action, int mods)>;

    window(const std::string &title, const int w, const int h);
    ~window();

    GLFWwindow *get_native() const { return _native_window; }
    void poll_events();
    bool should_close() const;
    void swap_buffers();

    void set_key_callback(key_callback_proxy cb) { _key_cb = std::move(cb); }
    void set_mouse_callback(mouse_move_callback_proxy cb) { _mouse_cb = std::move(cb); }
    void set_scroll_callback(scroll_callback_proxy cb) { _scroll_cb = std::move(cb); }
    void set_mouse_button_callback(mouse_button_callback_proxy cb) { _mouse_btn_cb = std::move(cb); }

    void set_fps(const int fps);
    void hide_cursor();

private:
    static void key_callback(GLFWwindow *wnd, int key, int scancode, int action, int mods);
    static void framebuffer_resize_callback(GLFWwindow *wnd, int width, int height);
    static void mouse_callback(GLFWwindow *wnd, double xpos, double ypos);
    static void scroll_callback(GLFWwindow *wnd, double xoffset, double yoffset);
    static void mouse_button_callback(GLFWwindow *wnd, int button, int action, int mods);

private:
    GLFWwindow *_native_window = nullptr;
    std::string _title;
    int _width = 0;
    int _height = 0;

    bool _cursors_hiden = false;
    key_callback_proxy _key_cb;
    mouse_move_callback_proxy _mouse_cb;
    scroll_callback_proxy _scroll_cb;
    mouse_button_callback_proxy _mouse_btn_cb;
};