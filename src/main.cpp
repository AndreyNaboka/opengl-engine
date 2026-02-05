
#include <glad/glad.h>
#include <glfw/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <memory>

#include "logger.h"
#include "shader.h"
#include "texture.h"
#include "settings.h"
#include "camera.h"
#include "game.h"
#include "window.h"
#include "input_manager.h"
#include "scene.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic pop

int main()
{
	input_manager input;
	input.bind_key(GLFW_KEY_W, input_manager::input_action::MOVE_FORWARD);
	input.bind_key(GLFW_KEY_S, input_manager::input_action::MOVE_BACKWARD);
	input.bind_key(GLFW_KEY_A, input_manager::input_action::MOVE_LEFT);
	input.bind_key(GLFW_KEY_D, input_manager::input_action::MOVE_RIGHT);
	input.bind_key(GLFW_KEY_ESCAPE, input_manager::input_action::QUIT);
	input.bind_key(GLFW_MOUSE_BUTTON_LEFT, input_manager::input_action::INTERACT);

	window main_wnd(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	main_wnd.set_key_callback([&input](int k, int s, int a, int m)
							  { input.on_key_event(k, a, m); });
	main_wnd.set_mouse_callback([&input](double x, double y)
								{ input.on_mouse_move(x, y); });
	main_wnd.set_scroll_callback([&input](double xoffset, double yoffset)
								 { input.on_mouse_scroll(xoffset, yoffset); });

	scene main_scene;

	camera main_camera;

	// // shaders
	// std::shared_ptr<shader> shader = shader::create("simple", textured_vert_shader, textured_frag_shader);
	// // textures
	// std::shared_ptr<texture> texture = texture::create("grass", "/Users/andreynaboka/code/assets/textures/Ground_01.png");
	// if (!texture)
	// 	return -1;
	// texture->load();

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

		const input_manager::mouse_state &cms = input.get_mouse_state();
		if (cms.delta_x != 0.0 || cms.delta_y != 0.0)
			main_camera.mouse_move(cms.delta_x, cms.delta_y);

		input.update();

		main_scene.pre_render();
		// main_scene.render();

		main_wnd.late_update();
		main_wnd.swap_buffers();

		game::instance().update();
		game::instance().end_update();
	}
	return 0;
}
