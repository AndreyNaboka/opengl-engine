#include "Renderer.h"
#include "Utils/Logger.h"
#include <algorithm>
#include <cstddef>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Renderer::SceneData Renderer::_sceneData;
std::vector<RenderCommand> Renderer::_cmdQueue;

void Renderer::Init() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
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

    cmd.shader->Bind();
    cmd.shader->SetUniformMat4("u_View", _sceneData.view);
    cmd.shader->SetUniformMat4("u_Projection", _sceneData.projection);
    cmd.shader->SetUniformMat4("u_Model", cmd.model);
    cmd.shader->SetUniformVec3("u_CameraPos", _sceneData.cameraPos);

    if (cmd.texture) {
      cmd.texture->Bind(cmd.slot);
      cmd.shader->SetUniformInt("u_Texture", cmd.slot);
    }

    const auto *animator = cmd.animator;
    const bool hasSkinning = animator && !animator->GetBoneMatrices().empty();

    if (hasSkinning) {
      cmd.shader->SetUniformInt("u_Skinned", 1);
      const auto &bones = animator->GetBoneMatrices();
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
  }
  _cmdQueue.clear();
}
