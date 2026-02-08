#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <memory>

#include "logger.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "window.h"
#include "input_manager.h"
#include "scene.h"
#include "timer.h"
#include "fps_counter.h"
#include "settings.h"

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
	input.bind_mouse_button(GLFW_MOUSE_BUTTON_LEFT, input_manager::input_action::INTERACT);

	window main_wnd(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	main_wnd.set_key_callback([&input](int k, int s, int a, int m)
							  { input.on_key_event(k, a, m); });
	main_wnd.set_mouse_callback([&input](double x, double y)
								{ input.on_mouse_move(x, y); });
	main_wnd.set_scroll_callback([&input](double xoffset, double yoffset)
								 { input.on_mouse_scroll(xoffset, yoffset); });
	main_wnd.set_mouse_button_callback([&input](int button, int action, int mods)
									   { input.on_mouse_button(button, action, mods); });

	scene main_scene;
	camera main_camera({0.0f, 2.5f, 3.0f});

	std::shared_ptr<shader> shader_phong = std::make_shared<shader>("/Users/andreynaboka/code/opengl-engine/assets/shaders/phong.vs", "/Users/andreynaboka/code/opengl-engine/assets/shaders/phong.fs");
	std::shared_ptr<texture> ground_texture = texture::create("ground_texture", "/Users/andreynaboka/code/opengl-engine/assets/img/Ground_01.png");

	ground_texture->load();

	mesh_renderer ground_mesh = main_scene.create_ground_mesh();
	ground_mesh.shader = shader_phong;
	ground_mesh.texture = ground_texture;

	transform ground_transform;
	main_scene.add_entity(ground_transform, ground_mesh);

	const glm::vec3 light_pos = {5.0f, 5.0f, 5.0f};

	timer timer;

	fps_counter fps_counter;
	auto last_log = std::chrono::high_resolution_clock::now();

	while (!main_wnd.should_close())
	{
		const float delta_time = timer.mark();

		// для физики использовать константный dt

		main_camera.set_delta_time(delta_time);

		main_wnd.poll_events();

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

		if (input.was_action_triggered(input_manager::input_action::INTERACT))
			main_wnd.hide_cursor();

		const input_manager::mouse_state &cms = input.get_mouse_state();
		if (cms.delta_x != 0.0 || cms.delta_y != 0.0)
			main_camera.mouse_move(cms.delta_x, cms.delta_y);

		input.update();

		main_scene.pre_render();
		main_scene.render(main_camera.get_view_matrix(),
						  main_camera.get_proj_matrix(),
						  main_camera.get_pos(),
						  main_camera.get_pos());

		main_wnd.swap_buffers();

		fps_counter.tick();
		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - last_log).count() >= 1)
		{
			main_wnd.set_fps(static_cast<int>(fps_counter.get_fps()));
			last_log = now;
		}
	}
	return 0;
}
