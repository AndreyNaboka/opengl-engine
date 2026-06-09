#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Animator.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>

enum class DepthFunc {
  Less,
  LessEqual,
};

struct RenderCommand {
  const Mesh *mesh = nullptr;
  const Shader *shader = nullptr;
  const Animator *animator = nullptr;
  const Texture *texture = nullptr;
  unsigned int slot = 0;
  glm::mat4 model = glm::mat4(1.0f);
  bool depthWrite = true; // запись в буфер глубины
  bool depthTest = true;  // тест глубины
  bool cullFace = true;   // отсечение задних граней
  DepthFunc depthFunc = DepthFunc::Less;
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
