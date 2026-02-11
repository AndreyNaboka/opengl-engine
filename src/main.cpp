#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <memory>

#include "Logger.h"
#include "shader.h"
#include "texture.h"
#include "Camera.h"
#include "Window.h"
#include "InputManager.h"
#include "Scene.h"
#include "Core/Timer.h"
#include "Core/FPSCounter.h"
#include "settings.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic pop

int main()
{
	InputManager input;
	input.BindKey(GLFW_KEY_W, InputManager::InputAction::MOVE_FORWARD);
	input.BindKey(GLFW_KEY_S, InputManager::InputAction::MOVE_BACKWARD);
	input.BindKey(GLFW_KEY_A, InputManager::InputAction::MOVE_LEFT);
	input.BindKey(GLFW_KEY_D, InputManager::InputAction::MOVE_RIGHT);
	input.BindKey(GLFW_KEY_ESCAPE, InputManager::InputAction::QUIT);
	input.BindMouseButton(GLFW_MOUSE_BUTTON_LEFT, InputManager::InputAction::INTERACT);

	Window mainWnd{WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT};
	mainWnd.SetKeyCallback([&input](int k, int s, int a, int m)
						   { input.OnKeyEvent(k, a, m); });
	mainWnd.SetMouseCallback([&input](double x, double y)
							 { input.OnMouseMove(x, y); });
	mainWnd.SetScrollCallback([&input](double xoffset, double yoffset)
							  { input.OnMouseScroll(xoffset, yoffset); });
	mainWnd.SetMouseButtonCallback([&input](int button, int action, int mods)
								   { input.OnMouseButton(button, action, mods); });

	Scene mainScene;
	Camera mainCamera({0.0f, 2.5f, 3.0f});

	std::shared_ptr<shader> shader_phong = std::make_shared<shader>("/Users/andreynaboka/code/opengl-engine/assets/shaders/phong.vs", "/Users/andreynaboka/code/opengl-engine/assets/shaders/phong.fs");
	std::shared_ptr<texture> ground_texture = texture::create("ground_texture", "/Users/andreynaboka/code/opengl-engine/assets/img/Ground_01.png");

	ground_texture->load();

	MeshRenderer groundMesh = mainScene.CreateGroundMesh();
	groundMesh.Shader = shader_phong;
	groundMesh.Texture = ground_texture;

	Transform groundTransform;
	mainScene.AddEntity(groundTransform, groundMesh);

	const glm::vec3 lightPos = {5.0f, 5.0f, 5.0f};

	Timer timer;

	FPSCounter fpsCounter;
	auto lastLogTime = std::chrono::high_resolution_clock::now();

	while (!mainWnd.ShouldClose())
	{
		const float delta_time = timer.mark();

		// для физики использовать константный dt

		mainCamera.SetDeltaTime(delta_time);

		mainWnd.PollEvents();

		if (input.WasActionTriggered(InputManager::InputAction::QUIT))
			break;

		if (input.IsActionActive(InputManager::InputAction::MOVE_FORWARD))
			mainCamera.MoveCamera(Camera::CameraDirection::FORWARD);

		if (input.IsActionActive(InputManager::InputAction::MOVE_BACKWARD))
			mainCamera.MoveCamera(Camera::CameraDirection::BACKWARD);

		if (input.IsActionActive(InputManager::InputAction::MOVE_LEFT))
			mainCamera.MoveCamera(Camera::CameraDirection::LEFT);

		if (input.IsActionActive(InputManager::InputAction::MOVE_RIGHT))
			mainCamera.MoveCamera(Camera::CameraDirection::RIGHT);

		if (input.WasActionTriggered(InputManager::InputAction::INTERACT))
			mainWnd.HideCursor();

		const InputManager::MouseState &currentMouseState = input.GetMouseState();
		if (currentMouseState.DeltaX != 0.0 || currentMouseState.DeltaY != 0.0)
			mainCamera.MouseMove(currentMouseState.DeltaX, currentMouseState.DeltaY);

		input.Update();

		mainScene.PreRender();
		mainScene.Render(mainCamera.GetViewMatrix(),
						 mainCamera.GetProjMatrix(),
						 mainCamera.GetPos(),
						 lightPos);

		mainWnd.SwapBuffers();

		fpsCounter.Tick();
		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - lastLogTime).count() >= 1)
		{
			mainWnd.SetFPS(static_cast<int>(fpsCounter.GetFPS()));
			lastLogTime = now;
		}
	}
	return 0;
}
