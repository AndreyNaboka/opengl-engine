#include "Renderer.h"
#include "Utils/Logger.h"
#include <algorithm>
#include <cstddef>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <tuple>

Renderer::SceneData Renderer::_sceneData;
std::vector<RenderCommand> Renderer::_cmdQueue;
RendererStats Renderer::_stats;
RenderStateMask Renderer::_currentState =
    RenderState::DepthTest | RenderState::DepthWrite | RenderState::CullFace;
DepthFunc Renderer::_currentDepthFunc = DepthFunc::Less;
unsigned int Renderer::_currentShaderID = 0;
std::array<Renderer::TextureState, Renderer::MaxTextureSlots>
    Renderer::_boundTextures;
unsigned int Renderer::_cameraUBO = 0;
unsigned int Renderer::_bonesUBO = 0;
bool Renderer::_initialized = false;

void Renderer::Init() {
  if (_initialized)
    return;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glCullFace(GL_BACK);

  glGenBuffers(1, &_cameraUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, _cameraUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(glm::vec4),
               nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, CameraBindingPoint, _cameraUBO);

  glGenBuffers(1, &_bonesUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, _bonesUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * MaxBones, nullptr,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, BonesBindingPoint, _bonesUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  _cmdQueue.reserve(128);
  _initialized = true;
}

void Renderer::Shutdown() {
  _cmdQueue.clear();
  if (_cameraUBO) {
    glDeleteBuffers(1, &_cameraUBO);
    _cameraUBO = 0;
  }
  if (_bonesUBO) {
    glDeleteBuffers(1, &_bonesUBO);
    _bonesUBO = 0;
  }
  _boundTextures.fill({});
  _currentShaderID = 0;
  _initialized = false;
}

void Renderer::BeginScene(const Camera &camera) {
  _stats = {};
  ResetFrameState();

  _sceneData.view = camera.GetViewMatrix();
  _sceneData.projection = camera.GetProjectionMatrix();
  _sceneData.cameraPos = camera.GetPosition();
  UploadCameraData();
  _cmdQueue.clear();
}

void Renderer::SetSunLight(const glm::vec3 &direction, const glm::vec3 &color,
                           float ambientStrength) {
  _sceneData.sunDirection = glm::normalize(direction);
  _sceneData.sunColor = color;
  _sceneData.ambientStrength = ambientStrength;
}

void Renderer::Submit(const RenderCommand &cmd) {
  _cmdQueue.push_back(cmd);
  ++_stats.commandsSubmitted;
}

void Renderer::EndScene() {
  glClearColor(0.1216f, 0.1216f, 0.2118f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  std::stable_sort(
      _cmdQueue.begin(), _cmdQueue.end(),
      [](const RenderCommand &a, const RenderCommand &b) {
        const unsigned int aShader = a.shader ? a.shader->GetID() : 0;
        const unsigned int bShader = b.shader ? b.shader->GetID() : 0;
        const unsigned int aTexture =
            !a.textures.empty() && a.textures[0].texture
                ? a.textures[0].texture->GetID()
                : 0;
        const unsigned int bTexture =
            !b.textures.empty() && b.textures[0].texture
                ? b.textures[0].texture->GetID()
                : 0;
        const unsigned int aMesh = a.mesh ? a.mesh->GetVAO() : 0;
        const unsigned int bMesh = b.mesh ? b.mesh->GetVAO() : 0;
        return std::tie(a.depthFunc, a.state, aShader, aTexture, aMesh) <
               std::tie(b.depthFunc, b.state, bShader, bTexture, bMesh);
      });

  for (const auto &cmd : _cmdQueue) {
    if (!cmd.mesh || !cmd.shader || !cmd.shader->IsValid()) {
      LogInfo("[Renderer] Skipping render command with invalid mesh or shader");
      continue;
    }
    ApplyRenderState(cmd);

    BindShader(*cmd.shader);
    cmd.shader->SetUniformMat4("u_Model", cmd.model);
    cmd.shader->SetUniformVec3("u_SunDirection", _sceneData.sunDirection);
    cmd.shader->SetUniformVec3("u_SunColor", _sceneData.sunColor);
    cmd.shader->SetUniformFloat("u_AmbientStrength",
                                _sceneData.ambientStrength);
    _stats.uniformUpdates += 4;

    for (const auto &binding : cmd.textures) {
      if (!binding.texture)
        continue;
      BindTexture(*binding.texture, binding.slot);
      cmd.shader->SetUniformInt(binding.samplerName, binding.slot);
      ++_stats.uniformUpdates;
    }
    if (cmd.skinning.IsValid()) {
      cmd.shader->SetUniformInt("u_Skinned", 1);
      ++_stats.uniformUpdates;
      const auto &bones = *cmd.skinning.boneMatrices;
      UploadBoneData(bones);
    } else {
      cmd.shader->SetUniformInt("u_Skinned", 0);
      ++_stats.uniformUpdates;
    }
    cmd.mesh->Draw();
    ++_stats.drawCalls;
    _stats.triangles += cmd.mesh->GetIndexCount() / 3;
  }
  _cmdQueue.clear();
}

const RendererStats &Renderer::GetStats() { return _stats; }

void Renderer::ResetFrameState() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);

  _currentState =
      RenderState::DepthTest | RenderState::DepthWrite | RenderState::CullFace;
  _currentDepthFunc = DepthFunc::Less;
  _currentShaderID = 0;
  _boundTextures.fill({});
}

void Renderer::ApplyRenderState(const RenderCommand &cmd) {
  if (cmd.depthFunc != _currentDepthFunc) {
    glDepthFunc(cmd.depthFunc == DepthFunc::LessEqual ? GL_LEQUAL : GL_LESS);
    _currentDepthFunc = cmd.depthFunc;
    ++_stats.stateChanges;
  }

  const auto applyFlag = [](RenderStateMask &currentState,
                            RenderStateMask targetState, RenderState flag,
                            unsigned int capability, RendererStats &stats) {
    const bool current = HasRenderState(currentState, flag);
    const bool target = HasRenderState(targetState, flag);
    if (current == target)
      return;

    if (target) {
      glEnable(capability);
      currentState = currentState | flag;
    } else {
      glDisable(capability);
      currentState &= ~static_cast<RenderStateMask>(flag);
    }
    ++stats.stateChanges;
  };

  applyFlag(_currentState, cmd.state, RenderState::DepthTest, GL_DEPTH_TEST,
            _stats);
  applyFlag(_currentState, cmd.state, RenderState::CullFace, GL_CULL_FACE,
            _stats);

  const bool currentDepthWrite =
      HasRenderState(_currentState, RenderState::DepthWrite);
  const bool targetDepthWrite =
      HasRenderState(cmd.state, RenderState::DepthWrite);
  if (currentDepthWrite != targetDepthWrite) {
    glDepthMask(targetDepthWrite ? GL_TRUE : GL_FALSE);
    if (targetDepthWrite)
      _currentState = _currentState | RenderState::DepthWrite;
    else
      _currentState &= ~static_cast<RenderStateMask>(RenderState::DepthWrite);
    ++_stats.stateChanges;
  }
}

void Renderer::BindShader(const Shader &shader) {
  if (_currentShaderID == shader.GetID())
    return;

  shader.Bind();
  _currentShaderID = shader.GetID();
  ++_stats.shaderBinds;
}

void Renderer::BindTexture(const BindableTexture &texture, unsigned int slot) {
  if (slot >= _boundTextures.size()) {
    texture.Bind(slot);
    ++_stats.textureBinds;
    return;
  }

  const unsigned int target = texture.GetTarget();
  const unsigned int id = texture.GetID();
  TextureState &current = _boundTextures[slot];
  if (current.target == target && current.id == id)
    return;

  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(target, id);
  current = {target, id};
  ++_stats.textureBinds;
}

void Renderer::UploadCameraData() {
  struct CameraBuffer {
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec4 cameraPos;
  };

  const CameraBuffer buffer{_sceneData.view, _sceneData.projection,
                            glm::vec4(_sceneData.cameraPos, 1.0f)};
  glBindBuffer(GL_UNIFORM_BUFFER, _cameraUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraBuffer), &buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  ++_stats.cameraUploads;
}

void Renderer::UploadBoneData(const std::vector<glm::mat4> &bones) {
  const size_t count = std::min(bones.size(), MaxBones);
  if (count == 0)
    return;

  glBindBuffer(GL_UNIFORM_BUFFER, _bonesUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * count,
                  bones.data());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  ++_stats.boneUploads;
}
