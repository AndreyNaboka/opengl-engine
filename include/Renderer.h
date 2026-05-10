#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <vector>

struct RenderCommand {
	const Mesh* mesh;
	const Shader* shader;
	const Texture* texture;
	unsigned int slot;
	glm::mat4 model;
};

class Renderer {
public:
  static void Init();
  static void BeginScene(const Camera &camera);
  static void Submit(const RenderCommand &cmd);
  static void EndScene();

private:
  struct SceneData {
	glm::mat4 view, projection;
	glm::vec3 cameraPos;
  };
  static SceneData _sceneData;
  static std::vector<RenderCommand> _cmdQueue;
};
