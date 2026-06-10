#include "Renderer.h"
#include "Utils/Logger.h"
#include <algorithm>
#include <cstddef>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

Renderer::SceneData Renderer::_sceneData;
std::vector<RenderCommand> Renderer::_cmdQueue;

void Renderer::Init() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glCullFace(GL_BACK);
  _cmdQueue.reserve(128);
}

void Renderer::BeginScene(const Camera &camera) {
  _sceneData.view = camera.GetViewMatrix();
  _sceneData.projection = camera.GetProjectionMatrix();
  _sceneData.cameraPos = camera.GetPosition();
  _cmdQueue.clear();
}

void Renderer::Submit(const RenderCommand &cmd) { _cmdQueue.push_back(cmd); }

void Renderer::EndScene() {
  glClearColor(0.1216f, 0.1216f, 0.2118f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const auto &cmd : _cmdQueue) {
    if (!cmd.mesh || !cmd.shader) {
      LogInfo("[Renderer] Skipping render command with null mesh or shader");
      continue;
    }
    if (cmd.depthFunc == DepthFunc::LessEqual) {
      glDepthFunc(GL_LEQUAL);
    }
    if (!HasRenderState(cmd.state, RenderState::DepthTest))
      glDisable(GL_DEPTH_TEST);
    if (!HasRenderState(cmd.state, RenderState::DepthWrite))
      glDepthMask(GL_FALSE);
    if (!HasRenderState(cmd.state, RenderState::CullFace))
      glDisable(GL_CULL_FACE);

    cmd.shader->Bind();
    cmd.shader->SetUniformMat4("u_View", _sceneData.view);
    cmd.shader->SetUniformMat4("u_Projection", _sceneData.projection);
    cmd.shader->SetUniformMat4("u_Model", cmd.model);
    cmd.shader->SetUniformVec3("u_CameraPos", _sceneData.cameraPos);

    for (const auto &binding : cmd.textures) {
      if (!binding.texture)
        continue;
      binding.texture->Bind(binding.slot);
      cmd.shader->SetUniformInt(binding.samplerName, binding.slot);
    }
    if (cmd.skinning.IsValid()) {
      cmd.shader->SetUniformInt("u_Skinned", 1);
      const auto &bones = *cmd.skinning.boneMatrices;
      if (!bones.empty()) {
        int loc = cmd.shader->GetUniformLocation("u_BoneMatrices");
        if (loc != -1) {
          constexpr size_t maxBones = 128;
          const size_t count = std::min(bones.size(), maxBones);
          glUniformMatrix4fv(loc, (GLsizei)count, GL_FALSE,
                             glm::value_ptr(bones[0]));
        }
      }
    } else {
      cmd.shader->SetUniformInt("u_Skinned", 0);
    }
    cmd.mesh->Draw();

    if (cmd.depthFunc == DepthFunc::LessEqual) {
      glDepthFunc(GL_LESS);
    }
    if (!HasRenderState(cmd.state, RenderState::DepthTest))
      glEnable(GL_DEPTH_TEST);
    if (!HasRenderState(cmd.state, RenderState::DepthWrite))
      glDepthMask(GL_TRUE);
    if (!HasRenderState(cmd.state, RenderState::CullFace))
      glEnable(GL_CULL_FACE);
  }
  _cmdQueue.clear();
}
