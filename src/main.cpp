#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "logger.h"

// Отключаем предупреждения от macOS о deprecated OpenGL
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <GLFW/glfw3.h>

#pragma clang diagnostic pop

void dump_system_info()
{
	const std::string version   = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	const std::string vendor    = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const std::string renderer  = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	const std::string glsl      = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

	logger::info("OpenGL info");
	logger::info("  version  :  " + version);
	logger::info("  vendor   :  " + vendor);
	logger::info("  renderer :  " + renderer);
	logger::info("  glsl     :  " + glsl);
}

int main() {
	if (!glfwInit()) {
		logger::error("Failed to initialize GLFW");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Wolrd", nullptr, nullptr);
	if (!window) {
		logger::error("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		logger::error("Failed to initialize GLAD");
		return -1;
	}

	dump_system_info();

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}