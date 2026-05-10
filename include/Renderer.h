#pragma once
#include "Camera.h"
#include <vector>

struct RenderCommand {};

class Renderer {
public:
  static void Init();
  static void BeginScene(const Camera &camera);
  static void Submit(const RenderCommand &cmd);
  static void EndScene();

private:
  struct SceneData {};
  static SceneData _sceneData;
  static std::vector<RenderCommand> _cmdQueue;
};
