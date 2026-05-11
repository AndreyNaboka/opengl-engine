#include <Renderer.h>
#include <glad/gl.h>

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
  glClearColor(0.1f, 0.15f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const auto &c : _cmdQueue) {
    c.shader->Bind();
    c.shader->SetUniformMat4("u_View", _sceneData.view);
    c.shader->SetUniformMat4("u_Projection", _sceneData.projection);
    c.shader->SetUniformMat4("u_Model", c.model);
    c.shader->SetUniformVec3("u_CameraPos", _sceneData.cameraPos);
    if (c.texture) {
      c.texture->Bind(c.slot);
      c.shader->SetUniformInt("u_Texture", c.slot);
    }
    c.mesh->Draw();
  }
  _cmdQueue.clear();
}
