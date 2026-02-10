#include "Window.h"
#include "Logger.h"
#include "settings.h"

static Window *GetWindowPtr(GLFWwindow *wnd)
{
    return static_cast<Window *>(glfwGetWindowUserPointer(wnd));
}

Window::Window(const std::string &title, const int w, const int h)
{
    if (!glfwInit())
    {
        Logger::Error("Failed to initialize GLFW");
        glfwTerminate();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    _nativeWindow = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!_nativeWindow)
    {
        Logger::Error("Failed to create GLFW window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(_nativeWindow);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(_nativeWindow, this);
    glfwSetFramebufferSizeCallback(_nativeWindow, FramebufferResizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Logger::Error("Failed to initialize GLAD");
        glfwTerminate();
    }

    int framebufferWidth = w;
    int framebufferHeight = h;
    glfwGetFramebufferSize(_nativeWindow, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glfwSetKeyCallback(_nativeWindow, KeyCallback);
    glfwSetCursorPosCallback(_nativeWindow, MouseCallback);
    glfwSetScrollCallback(_nativeWindow, ScrollCallback);
    glfwSetMouseButtonCallback(_nativeWindow, MouseButtonCallback);
}

void Window::FramebufferResizeCallback(GLFWwindow *wnd, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::MouseButtonCallback(GLFWwindow *wnd, int button, int action, int mods)
{
    if (auto *self = GetWindowPtr(wnd))
        if (self->_mouseBtnCb)
            self->_mouseBtnCb(button, action, mods);
}

void Window::MouseCallback(GLFWwindow *wnd, double xpos, double ypos)
{
    if (auto *self = GetWindowPtr(wnd))
        if (self->_mouseCb)
            self->_mouseCb(xpos, ypos);
}

void Window::ScrollCallback(GLFWwindow *wnd, double xoffset, double yoffset)
{
    if (auto *self = GetWindowPtr(wnd))
        if (self->_scrollCb)
            self->_scrollCb(xoffset, yoffset);
}

void Window::KeyCallback(GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
    if (auto *self = GetWindowPtr(wnd))
        if (self->_keyCb)
            self->_keyCb(key, scancode, action, mods);
}

Window::~Window()
{
    glfwDestroyWindow(_nativeWindow);
    glfwTerminate();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(_nativeWindow);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(_nativeWindow);
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SetFPS(const int fps)
{
    char title[100];
    snprintf(title, 100, "%s FPS: %i", WINDOW_TITLE, fps);
    glfwSetWindowTitle(_nativeWindow, title);
}

void Window::HideCursor()
{
    _showCursor = !_showCursor;
    glfwSetInputMode(_nativeWindow, GLFW_CURSOR, _showCursor == false ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}