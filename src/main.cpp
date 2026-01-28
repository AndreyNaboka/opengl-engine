
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
#include "window.h"
#include "input_manager.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#pragma clang diagnostic pop

void render_prepare(GLuint &ret_vao, GLuint &ret_vbo);

int main()
{
	input_manager input;
	input.bind_key(GLFW_KEY_W, input_manager::input_action::MOVE_FORWARD);
	input.bind_key(GLFW_KEY_S, input_manager::input_action::MOVE_BACKWARD);
	input.bind_key(GLFW_KEY_A, input_manager::input_action::MOVE_LEFT);
	input.bind_key(GLFW_KEY_D, input_manager::input_action::MOVE_RIGHT);
	input.bind_key(GLFW_KEY_ESCAPE, input_manager::input_action::QUIT);

	window main_wnd(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	main_wnd.set_key_callback([&input](int k, int s, int a, int m)
							  { input.on_key_event(k, a, m); });

	camera main_camera;

	GLuint vao = 0;
	GLuint vbo = 0;
	render_prepare(vao, vbo);

	// shaders
	std::shared_ptr<shader> shader = shader::create("simple", textured_vert_shader, textured_frag_shader);
	// textures
	std::shared_ptr<texture> texture = texture::create("grass", "/Users/andreynaboka/code/assets/textures/Ground_01.png");
	if (!texture)
		return -1;
	texture->load();

	// models positions
	constexpr glm::vec3 cube_positions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)};

	// transform matrix prepare
	const glm::mat4 projection = main_camera.get_proj_matrix();

	while (!main_wnd.should_close())
	{
		main_wnd.poll_events();
		game::instance().begin_update();

		if (input.was_action_triggered(input_manager::input_action::QUIT))
			break;

		if (input.is_action_active(input_manager::input_action::MOVE_FORWARD))
			main_camera.move_camera(camera::camera_direction::FORWARD);

		if (input.is_action_active(input_manager::input_action::MOVE_BACKWARD))
			main_camera.move_camera(camera::camera_direction::BACKWARD);

		if (input.is_action_active(input_manager::input_action::MOVE_LEFT))
			main_camera.move_camera(camera::camera_direction::LEFT);

		if (input.is_action_active(input_manager::input_action::MOVE_RIGHT))
			main_camera.move_camera(camera::camera_direction::RIGHT);

		input.update();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		texture->bind();

		const GLuint model_uniform = shader->get_uniform_loc("model");
		const GLuint view_uniform = shader->get_uniform_loc("view");
		const GLuint projection_uniform = shader->get_uniform_loc("projection");

		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(main_camera.get_view_matrix()));
		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

		shader->bind();

		glBindVertexArray(vao);
		constexpr unsigned int count_of_models = 10;
		for (int i = 0; i < count_of_models; i++)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), cube_positions[i]);
			const GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

			glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		main_wnd.late_update();
		main_wnd.swap_buffers();

		game::instance().update();
		game::instance().end_update();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	return 0;
}

void render_prepare(GLuint &ret_vao, GLuint &ret_vbo)
{
	constexpr GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

	glEnable(GL_DEPTH_TEST);

	GLuint vbo = 0;
	GLuint vao = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	ret_vao = vao;
	ret_vbo = vbo;
}
