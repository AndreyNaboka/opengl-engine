
#include <glad/glad.h>
#include <glfw/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <memory>

#include "logger.h"
#include "shaders_lib.h"
#include "shader.h"
#include "texture.h"
#include "settings.h"
#include "camera.h"
#include "game.h"
#include "light.h"
#include "window.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#pragma clang diagnostic pop

// declare fusnctions ---------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void render_prepare(GLuint& ret_vao, GLuint& ret_vbo);
void set_viewport(GLFWwindow* window);
void dump_system_info();
void update_fps(GLFWwindow* window);
void update_camera();
// ---------------------------------

// global vars ---------------------
std::shared_ptr<camera> main_camera;
std::shared_ptr<light> global_light;
bool keys[1024] = {};
float last_x = WINDOW_WIDTH  * 0.5f;
float last_y = WINDOW_HEIGHT * 0.5f;
bool first_mouse_move = true;
// ---------------------------------

int main() 
{
	window::wnd_ptr main_wnd = *(window::create(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT));
	if (!main_wnd) {
		logger::error("Can't create main window. Exit.");
		return EXIT_FAILURE;	
	}

	main_camera = std::make_shared<camera>();
	global_light = std::make_shared<light>(glm::vec3(1.2f, 1.0f, 2.0f), "main");

	dump_system_info();

	GLuint vao = 0;
	GLuint vbo = 0;
	render_prepare(vao, vbo);

	// shaders
	std::shared_ptr<shader> shader = shader::create("simple", textured_vert_shader, textured_frag_shader);
	// textures
	std::shared_ptr<texture> texture = texture::create("grass", "/Users/andreynaboka/code/assets/textures/Ground_01.png");
	if (!texture) return -1;
	texture->load();

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
	const glm::mat4 projection = main_camera->get_proj_matrix();

	while (!main_wnd->should_close()) {
		main_wnd->poll_events();

		update_camera();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// light render------------------------------------------------ 
		global_light->render();
		// ------------------------------------------------------------


		// draw objects------------------------------------------------
		texture->bind();

		// shader uniforms update--------------------------------------
		const GLuint model_uniform = shader->get_uniform_loc("model");
		const GLuint view_uniform  = shader->get_uniform_loc("view");
		const GLuint projection_uniform = shader->get_uniform_loc("projection");

		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(main_camera->get_view_matrix()));
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

		update_fps(main_wnd->get_native());
		game::instance().update();

		main_wnd->swap_buffers();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
	return 0;
}

void update_camera()
{
	if (keys[GLFW_KEY_W]) main_camera->move_camera(camera::camera_direction::FORWARD);
	if (keys[GLFW_KEY_S]) main_camera->move_camera(camera::camera_direction::BACKWARD);
	if (keys[GLFW_KEY_A]) main_camera->move_camera(camera::camera_direction::LEFT);
	if (keys[GLFW_KEY_D]) main_camera->move_camera(camera::camera_direction::RIGHT);
}

void update_fps(GLFWwindow* window) 
{
	const float current_fps = game::instance().get_fps();
	if (current_fps == 0.0f) return;
	const std::string title = "World fps " + std::to_string(current_fps);
	glfwSetWindowTitle(window, title.c_str());
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (first_mouse_move) {
		last_x = xpos;
		last_y = ypos;
		first_mouse_move = false;
	}

	const float x_offset = xpos - last_x;
	const float y_offset = last_y - ypos;

	last_x = xpos;
	last_y = ypos;

	main_camera->mouse_move(x_offset, y_offset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	main_camera->mouse_move(xoffset, yoffset);
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
