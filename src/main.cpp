// src/main.cpp
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

	Logger::info("OpenGL info");
	Logger::info("  version  :  " + version);
	Logger::info("  vendor   :  " + vendor);
	Logger::info("  renderer :  " + renderer);
	Logger::info("  glsl     :  " + glsl);
}

int main() {
	// Инициализация GLFW
	if (!glfwInit()) {
		Logger::error("Failed to initialize GLFW");
		return -1;
	}

	// Настройка контекста OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Создаём окно
	GLFWwindow* window = glfwCreateWindow(800, 600, "Wolrd", nullptr, nullptr);
	if (!window) {
		Logger::error("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Загружаем OpenGL функции через GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Logger::error("Failed to initialize GLAD");
		return -1;
	}

	dump_system_info();

	// Главный цикл
	while (!glfwWindowShouldClose(window)) {
		// Очистка экрана
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// События и буфер
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
