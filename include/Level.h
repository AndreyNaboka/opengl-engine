#pragma once

#include "Animator.h"
#include "CubeMapTexture.h"
#include "GltfLoader.h"
#include "Mesh.h"
#include "PhysicsWorld.h"
#include "PlayerController.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>

class Camera;
class InputManager;

class Level {
public:
  Level();
  ~Level();

  void Update(const InputManager &input, Camera &camera, float dt);
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
  float _enemyOrbitAngle = 0.0f;

  std::shared_ptr<CubeMapTexture> _skyboxTexture;
  std::shared_ptr<Shader> _skyboxShader;
  std::shared_ptr<Mesh> _skyboxMesh;
  RenderCommand _skyboxCmd;

  PhysicsWorld _physicsWorld;
  PlayerController _playerController;

  void SetupTerrain();
  void SetupPlayer();
  void SetupModel();
  void SetupSkybox();
};
