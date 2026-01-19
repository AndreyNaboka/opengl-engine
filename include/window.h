#pragma once
#include <string>
#include <memory>
#include "glad/glad.h"
#include <glfw/glfw3.h>
#include <optional>

class window
{
public:
    using wnd_ptr = std::shared_ptr<window>;

public:
    static std::optional<window::wnd_ptr> create(const std::string& title, const int w, const int h);
    GLFWwindow* get_native() const { return _window; }
    void poll_events();
    bool should_close() const;
    void swap_buffers();
    ~window();

private:
    window(GLFWwindow* wnd, const std::string& title, const int w, const int h);
    static void key_callback(GLFWwindow* wnd, int key, int scancode, int action, int mods);
    static void framebuffer_resize_callback(GLFWwindow* wnd, int width, int height);
    static void mouse_callback(GLFWwindow* wnd, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* wnd, double xoffset, double yoffset);
private:
    GLFWwindow* _window = nullptr;
    std::string _title;
    int _width = 0;
    int _height = 0;
};