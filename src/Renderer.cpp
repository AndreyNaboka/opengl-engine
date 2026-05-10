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
  // Set the scene data
  _cmdQueue.clear();
}

void Renderer::Submit(const RenderCommand &cmd) { _cmdQueue.push_back(cmd); }

void Renderer::EndScene() {
  glClearColor(0.1f, 0.35f, 0.65f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _cmdQueue.clear();
}
