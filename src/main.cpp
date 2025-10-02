#include <glad/glad.h>
#include <glfw/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <memory>
#include <cmath>

#include "logger.h"
#include "shaders_lib.h"
#include "shader.h"
#include "texture.h"
#include "settings.h"
#include "camera.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#pragma clang diagnostic pop

// declare functions ---------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void render_prepare(GLuint& ret_vao, GLuint& ret_vbo);
void set_viewport(GLFWwindow* window);
void dump_system_info();
// ---------------------------------

// global vars ---------------------
bool keys[1024];
std::shared_ptr<camera> main_camera;
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

	main_camera.reset(new camera());

	dump_system_info();
	set_viewport(window);

	GLuint vao = 0;
	GLuint vbo = 0;
	render_prepare(vao, vbo);

	// shaders
	std::shared_ptr<shader> shader = shader::create("simple", textured_vert_shader, textured_frag_shader);
	// textures
	std::shared_ptr<texture> texture = texture::create("grass", "/Users/andreynaboka/code/assets/textures/Grass_01.png");
	if (!texture) return -1;
	texture->load();
	// transform matrix
	// glm::mat4 transform_mat = glm::mat4(1.0f);
	// transform_mat = glm::rotate(transform_mat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	// transform_mat = glm::scale(transform_mat, glm::vec3(0.5, 0.5, 0.5));  
	// float rot_angle = 0.0f;

	double previous_time = glfwGetTime();

	// models positions 
	constexpr glm::vec3 cube_positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// transform matrix prepare
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	const glm::mat4 projection = main_camera->get_proj_matrix();

	while (!glfwWindowShouldClose(window)) {
		const double current_time = glfwGetTime();
		const double delta_time = current_time - previous_time;

		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		texture->bind();

		// shader uniforms update--------------------------------------
		const GLuint model_uniform = shader->get_uniform_loc("model");
		const GLuint view_uniform  = shader->get_uniform_loc("view");
		const GLuint projection_uniform = shader->get_uniform_loc("projection");

		const GLfloat radius = 10.0f;
		const GLfloat cam_x = sin(glfwGetTime()) * radius;
		const GLfloat cam_z = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(cam_x, 0.0, cam_z), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

		shader->bind();
		// ------------------------------------------------------------

		// draw models ------------------------------------------------
		glBindVertexArray(vao);
		constexpr unsigned int count_of_models = 10;
		for (int i = 0; i < count_of_models; i++) {
			glm::mat4 model = glm::translate(glm::mat4(1.0f), cube_positions[i]);
			const GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

			glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);
		// ------------------------------------------------------------

		glfwSwapBuffers(window);

		previous_time = current_time;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void render_prepare(GLuint& ret_vao, GLuint& ret_vbo)
{
	constexpr GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glEnable(GL_DEPTH_TEST);

	GLuint vbo = 0;
	GLuint vao = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

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