#pragma once
#include "Camera.h"
#include "Shader.h"
#include "BindableTexture.h"
#include "Mesh.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <vector>

enum class DepthFunc {
  Less,
  LessEqual,
};

enum class RenderState : uint32_t {
  None = 0,
  DepthTest = 1 << 0,
  DepthWrite = 1 << 1,
  CullFace = 1 << 2,
};

using RenderStateMask = uint32_t;

constexpr RenderStateMask operator|(RenderState a, RenderState b) {
  return static_cast<RenderStateMask>(a) | static_cast<RenderStateMask>(b);
}

constexpr RenderStateMask operator|(RenderStateMask mask, RenderState flag) {
  return mask | static_cast<RenderStateMask>(flag);
}

constexpr bool HasRenderState(RenderStateMask mask, RenderState flag) {
  return (mask & static_cast<RenderStateMask>(flag)) != 0;
}

struct TextureBinding {
  const BindableTexture *texture = nullptr;
  std::string samplerName = "u_Texture";
  unsigned int slot = 0;
};

struct SkinningBinding {
  const std::vector<glm::mat4> *boneMatrices = nullptr;
  bool IsValid() const { return boneMatrices && !boneMatrices->empty(); }
};

struct RenderCommand {
  const Mesh *mesh = nullptr;
  const Shader *shader = nullptr;
  SkinningBinding skinning;
  std::vector<TextureBinding> textures;
  glm::mat4 model = glm::mat4(1.0f);
  RenderStateMask state = RenderState::DepthTest | RenderState::DepthWrite |
                          RenderState::CullFace;
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
