#pragma once
#include "Camera.h"
#include "Shader.h"
#include "BindableTexture.h"
#include "Mesh.h"
#include "Animator.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

enum class DepthFunc {
  Less,
  LessEqual,
};

struct TextureBinding {
  const BindableTexture *texture = nullptr;
  std::string samplerName = "u_Texture";
  unsigned int slot = 0;
};

struct RenderCommand {
  const Mesh *mesh = nullptr;
  const Shader *shader = nullptr;
  const Animator *animator = nullptr;
  std::vector<TextureBinding> textures;
  glm::mat4 model = glm::mat4(1.0f);
  bool depthWrite = true;
  bool depthTest = true;
  bool cullFace = true;
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
