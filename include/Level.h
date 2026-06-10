#pragma once

#include "Animator.h"
#include "CubeMapTexture.h"
#include "GltfLoader.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>

class Level {
public:
  Level();

  void Update(float dt);
  void Render() const;

private:
  std::unique_ptr<Shader> _terrainShader;
  std::unique_ptr<Texture> _groundTexture;
  std::unique_ptr<Mesh> _terrainMesh;
  RenderCommand _groundCmd;

  GltfModelData _modelData;
  std::unique_ptr<Shader> _modelShader;
  Animator _animator;
  RenderCommand _modelCmd;

  std::shared_ptr<CubeMapTexture> _skyboxTexture;
  std::shared_ptr<Shader> _skyboxShader;
  std::shared_ptr<Mesh> _skyboxMesh;
  RenderCommand _skyboxCmd;
};
