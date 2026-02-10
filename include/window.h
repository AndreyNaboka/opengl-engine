#pragma once
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <functional>

#include "glad/glad.h"
#include <glfw/glfw3.h>

class Window
{
public:
    using wndPtr = std::shared_ptr<Window>;

public:
    using KeyCallbackProxy = std::function<void(int key, int scancode, int action, int mods)>;
    using MouseMoveCallbackProxy = std::function<void(double xpos, double ypos)>;
    using ScrollCallbackProxy = std::function<void(double xoffset, double yoffset)>;
    using MouseButtonCallbackProxy = std::function<void(int button, int action, int mods)>;

    Window(const std::string &title, const int w, const int h);
    ~Window();

    GLFWwindow *GetNative() const { return _nativeWindow; }
    void PollEvents();
    bool ShouldClose() const;
    void SwapBuffers();

    void SetKeyCallback(KeyCallbackProxy cb) { _keyCb = std::move(cb); }
    void SetScrollCallback(ScrollCallbackProxy cb) { _scrollCb = std::move(cb); }
    void SetMouseCallback(MouseMoveCallbackProxy cb) { _mouseCb = std::move(cb); } 
    void SetMouseButtonCallback(MouseButtonCallbackProxy cb) { _mouseBtnCb = std::move(cb); }

    void SetFPS(const int fps);
    void HideCursor();

private:
    static void KeyCallback(GLFWwindow *wnd, int key, int scancode, int action, int mods);
    static void FramebufferResizeCallback(GLFWwindow *wnd, int width, int height);
    static void MouseCallback(GLFWwindow *wnd, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow *wnd, double xoffset, double yoffset);
    static void MouseButtonCallback(GLFWwindow *wnd, int button, int action, int mods);

private:
    int _width = 0;
    int _height = 0;
    std::string _title;
    GLFWwindow *_nativeWindow = nullptr;

    bool _showCursor = true;
    KeyCallbackProxy _keyCb;
    MouseMoveCallbackProxy _mouseCb;
    ScrollCallbackProxy _scrollCb;
    MouseButtonCallbackProxy _mouseBtnCb;
};