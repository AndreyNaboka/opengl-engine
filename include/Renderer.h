#pragma once
#include "Camera.h"
#include "Shader.h"
#include "BindableTexture.h"
#include "Mesh.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <array>
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
  RenderStateMask state =
      RenderState::DepthTest | RenderState::DepthWrite | RenderState::CullFace;
  DepthFunc depthFunc = DepthFunc::Less;
};

struct RendererStats {
  uint32_t commandsSubmitted = 0;
  uint32_t drawCalls = 0;
  uint32_t shaderBinds = 0;
  uint32_t textureBinds = 0;
  uint32_t stateChanges = 0;
  uint32_t uniformUpdates = 0;
  uint32_t cameraUploads = 0;
  uint32_t boneUploads = 0;
  uint32_t triangles = 0;
};

class Renderer {
public:
  static void Init();
  static void SetSunLight(const glm::vec3 &direction, const glm::vec3 &color,
                          float ambientStrength);
  static void BeginScene(const Camera &camera);
  static void Submit(const RenderCommand &cmd);
  static void EndScene();
  static const RendererStats &GetStats();

private:
  static constexpr unsigned int CameraBindingPoint = 0;
  static constexpr unsigned int BonesBindingPoint = 1;
  static constexpr size_t MaxTextureSlots = 16;
  static constexpr size_t MaxBones = 128;

  struct SceneData {
    glm::mat4 view, projection;
    glm::vec3 cameraPos;
    glm::vec3 sunDirection = glm::vec3(-0.45f, -0.85f, -0.25f);
    glm::vec3 sunColor = glm::vec3(1.0f, 0.86f, 0.62f);
    float ambientStrength = 0.16f;
  };

  struct TextureState {
    unsigned int target = 0;
    unsigned int id = 0;
  };

  static SceneData _sceneData;
  static std::vector<RenderCommand> _cmdQueue;
  static RendererStats _stats;
  static RenderStateMask _currentState;
  static DepthFunc _currentDepthFunc;
  static unsigned int _currentShaderID;
  static std::array<TextureState, MaxTextureSlots> _boundTextures;
  static unsigned int _cameraUBO;
  static unsigned int _bonesUBO;

  static void ResetFrameState();
  static void ApplyRenderState(const RenderCommand &cmd);
  static void BindShader(const Shader &shader);
  static void BindTexture(const BindableTexture &texture, unsigned int slot);
  static void UploadCameraData();
  static void UploadBoneData(const std::vector<glm::mat4> &bones);
};
