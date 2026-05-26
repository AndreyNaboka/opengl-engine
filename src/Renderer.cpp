#include <Renderer.h>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

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
  glClearColor(0.0f, 0.15f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const auto &cmd : _cmdQueue) {
    cmd.shader->Bind();
    cmd.shader->SetUniformMat4("u_View", _sceneData.view);
    cmd.shader->SetUniformMat4("u_Projection", _sceneData.projection);
    cmd.shader->SetUniformMat4("u_Model", cmd.model);
    cmd.shader->SetUniformVec3("u_CameraPos", _sceneData.cameraPos);

    if (cmd.texture) {
      cmd.texture->Bind(cmd.slot);
      cmd.shader->SetUniformInt("u_Texture", cmd.slot);
    }

    if (cmd.animator && cmd.animator->IsPlaying()) {
      cmd.shader->SetUniformInt("u_Skinned", 1);
      const auto &bones = cmd.animator->GetBoneMatrices();
      if (!bones.empty()) {
        int loc = cmd.shader->GetUniformLocation("u_BoneMatrices");
        if (loc != -1) {
          glUniformMatrix4fv(loc, static_cast<GLsizei>(bones.size()), GL_FALSE,
                             glm::value_ptr(bones[0]));
        }
      }
    } else {
      cmd.shader->SetUniformInt("u_Skinned", 0);
    }
    glDisable(GL_CULL_FACE);
    cmd.mesh->Draw();
    glEnable(GL_CULL_FACE);
  }
  _cmdQueue.clear();
}
