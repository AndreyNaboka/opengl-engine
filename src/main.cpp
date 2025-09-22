#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <memory>
#include <cmath>

#include "logger.h"
#include "shaders_lib.h"
#include "shader.h"
#include "texture.h"
#include "settings.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#pragma clang diagnostic pop

// declare functions ---------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void render_prepare(GLuint& ret_vao, GLuint& ret_vbo);
void set_viewport(GLFWwindow* window);
void dump_system_info();
// ---------------------------------

int main() 
{
	if (!glfwInit()) {
		logger::error("Failed to initialize GLFW");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);

	if (!window) {
		logger::error("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		logger::error("Failed to initialize GLAD");
		return -1;
	}

	dump_system_info();
	set_viewport(window);

	std::shared_ptr<shader> shader = shader::create("simple", simple_vert_shader, simple_frag_shader);

	GLuint vao = 0;
	GLuint vbo = 0;
	render_prepare(vao, vbo);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// animate shader
		const GLfloat time_value = glfwGetTime();
		const GLfloat progress = (std::sin(time_value * 0.5) * 0.5f) + 0.5f;
		const GLint u_time = shader->get_uniform_loc("u_time");
		shader->bind_shader();
		glUniform1f(u_time, progress);
		
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void render_prepare(GLuint& ret_vao, GLuint& ret_vbo)
{
	constexpr GLfloat vertices[] = {
       0.5f,  0.5f, 0.0f,  // Верхний правый угол
    	 0.5f, -0.5f, 0.0f,  // Нижний правый угол
    	-0.5f, -0.5f, 0.0f,  // Нижний левый угол
    	-0.5f,  0.5f, 0.0f   // Верхний левый угол
	};

	constexpr GLuint indices[] = {
   	0, 1, 3,   // Первый треугольник
    	1, 2, 3    // Второй треугольник
	};  

	GLuint vbo = 0;
	GLuint vao = 0;
	GLuint ibo = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	ret_vao = vao;
	ret_vbo = vbo;
}

void set_viewport(GLFWwindow* window)
{
	int framebuffer_width = WINDOW_WIDTH;
	int frame_buffer_height = WINDOW_HEIGHT;
	glfwGetFramebufferSize(window, &framebuffer_width, &frame_buffer_height);
	glViewport(0, 0, framebuffer_width, frame_buffer_height);
}

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