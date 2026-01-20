#include "window.h"
#include "logger.h"

std::optional<window::wnd_ptr> window::create(const std::string& title, const int w, const int h)
{
    if (w <= 0 || h <= 0) return std::nullopt;

    if (!glfwInit()) {
		logger::error("Failed to initialize GLFW");
		return std::nullopt;
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* native_window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!native_window) {
		logger::error("Failed to create GLFW window");
		glfwTerminate();
		return std::nullopt;
	}

	glfwMakeContextCurrent(native_window);
	glfwSetKeyCallback(native_window, key_callback);
	glfwSetCursorPosCallback(native_window, mouse_callback);
	glfwSetScrollCallback(native_window, scroll_callback);
    glfwSetFramebufferSizeCallback(native_window, framebuffer_resize_callback);
	glfwSetInputMode(native_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		logger::error("Failed to initialize GLAD");
		return std::nullopt;
	}

    int framebuffer_width = w;
	int frame_buffer_height = h;
	glfwGetFramebufferSize(native_window, &framebuffer_width, &frame_buffer_height);
	glViewport(0, 0, framebuffer_width, frame_buffer_height);

    window::wnd_ptr w_ptr(new window(native_window, title, w, h));
    glfwSetWindowUserPointer(native_window, w_ptr.get());
    return w_ptr;
}

void window::key_callback(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(wnd, true);

    auto* self = static_cast<window*>(glfwGetWindowUserPointer(wnd));
    if (!self) { 
        logger::info("Can't get glfwGetWindowUserPointer!");
        return;
    }

    for (auto &subscriber: self->_input_subscribers) {
        subscriber->on_key(key, scancode, action, mods);
    }
}

void window::framebuffer_resize_callback(GLFWwindow* wnd, int width, int height)
{
    glViewport(0, 0, width, height);
}

void window::mouse_callback(GLFWwindow* wnd, double xpos, double ypos)
{
}

void window::scroll_callback(GLFWwindow* wnd, double xoffset, double yoffset)
{
}

window::window(GLFWwindow* wnd, const std::string& title, const int w, const int h) 
: _window(wnd) 
, _title(title)
, _width(w)
, _height(h)
{
    _input_subscribers.reserve(10);
}

window::~window()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void window::swap_buffers() { glfwSwapBuffers(_window); }

bool window::should_close() const { return glfwWindowShouldClose(_window); }

void window::poll_events() { glfwPollEvents(); }