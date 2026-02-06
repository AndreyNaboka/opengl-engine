#include "window.h"
#include "game.h"
#include "logger.h"

static window *get_window_ptr(GLFWwindow *wnd)
{
    return static_cast<window *>(glfwGetWindowUserPointer(wnd));
}

window::window(const std::string &title, const int w, const int h)
{
    if (!glfwInit())
    {
        logger::error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    _native_window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!_native_window)
    {
        logger::error("Failed to create GLFW window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(_native_window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(_native_window, this);
    glfwSetFramebufferSizeCallback(_native_window, framebuffer_resize_callback);
    glfwSetInputMode(_native_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        logger::error("Failed to initialize GLAD");

    int framebuffer_width = w;
    int frame_buffer_height = h;
    glfwGetFramebufferSize(_native_window, &framebuffer_width, &frame_buffer_height);
    glViewport(0, 0, framebuffer_width, frame_buffer_height);

    glfwSetKeyCallback(_native_window, key_callback);
    glfwSetCursorPosCallback(_native_window, mouse_callback);
    glfwSetScrollCallback(_native_window, scroll_callback);
}

void window::framebuffer_resize_callback(GLFWwindow *wnd, int width, int height)
{
    glViewport(0, 0, width, height);
}

void window::mouse_callback(GLFWwindow *wnd, double xpos, double ypos)
{
    if (auto *self = get_window_ptr(wnd))
        if (self->_mouse_cb)
            self->_mouse_cb(xpos, ypos);
}

void window::scroll_callback(GLFWwindow *wnd, double xoffset, double yoffset)
{
    if (auto *self = get_window_ptr(wnd))
        if (self->_scroll_cb)
            self->_scroll_cb(xoffset, yoffset);
}

void window::key_callback(GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
    if (auto *self = get_window_ptr(wnd))
        if (self->_key_cb)
            self->_key_cb(key, scancode, action, mods);
}

window::~window()
{
    glfwDestroyWindow(_native_window);
    glfwTerminate();
}

void window::swap_buffers()
{
    glfwSwapBuffers(_native_window);
}

bool window::should_close() const
{
    return glfwWindowShouldClose(_native_window);
}

void window::poll_events()
{
    glfwPollEvents();
}

void window::late_update()
{
    static auto last_log = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - last_log).count() < 1)
        return;

    last_log = now;
    // update fps in window title
    const int current_fps = game::instance().get_fps();
    if (current_fps == 0)
        return;
    const std::string title = "World fps " + std::to_string(current_fps);
    glfwSetWindowTitle(get_native(), title.c_str());
}
