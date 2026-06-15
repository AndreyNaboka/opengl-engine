#pragma once

#include "Animator.h"
#include "CubeMapTexture.h"
#include "GltfLoader.h"
#include "Mesh.h"
#include "PhysicsWorld.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include <Jolt/Physics/Body/BodyID.h>
#include <memory>

class Camera;
class InputManager;

class Level {
public:
  Level();
  ~Level();

  void Update(const InputManager &input, Camera &camera, float dt);
  void Render() const;
  bool IsFreeCameraMode() const { return _freeCameraMode; }

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
  JPH::BodyID _groundBody;
  JPH::BodyID _playerBody;
  bool _playerGrounded = false;
  bool _freeCameraMode = false;

  void UpdatePlayer(const InputManager &input, const Camera &camera);
  void SyncPlayerToCamera(const Camera &camera);
  glm::vec3 GetPlayerEyePosition();
};
