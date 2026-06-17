#include "Level.h"
#include "Camera.h"
#include "InputManager.h"
#include "Sky.h"
#include "TerrainGenerator.h"
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/EActivation.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <string>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace {
constexpr float kEnemyScale = 6.0f;
constexpr float kEnemyGroundOffsetY = 2.0f;
constexpr float kEnemyOrbitRadius = 18.0f;
constexpr float kEnemyOrbitSpeed = 0.65f;
constexpr float kGroundSize = 420.0f;
constexpr float kPlayerCapsuleHalfHeight = 0.9f;
constexpr float kPlayerCapsuleRadius = 0.35f;
constexpr float kPlayerStandingCenterY =
    kPlayerCapsuleHalfHeight + kPlayerCapsuleRadius;
constexpr float kPlayerEyeOffsetY = 7.45f;
constexpr float kPlayerMoveSpeed = 10.0f;
constexpr float kPlayerJumpSpeed = 10.5f;
constexpr float kGroundedTolerance = 0.08f;

glm::mat4 EnemyOrbitTransform(float angle) {
  const glm::vec3 position(std::cos(angle) * kEnemyOrbitRadius,
                           kEnemyGroundOffsetY,
                           std::sin(angle) * kEnemyOrbitRadius);

  const glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
  const glm::mat4 rotate =
      glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(0.0f, 1.0f, 0.0f));
  const glm::mat4 scale =
      glm::scale(glm::mat4(1.0f), glm::vec3(kEnemyScale));

  return translate * rotate * scale;
}
} // namespace

Level::Level() {
  _terrainShader = std::make_unique<Shader>("assets/shaders/terrain.vert",
                                            "assets/shaders/common.frag");
  _groundTexture = std::make_unique<Texture>("assets/textures/grass.png");
  _terrainMesh = GenerateGrid(kGroundSize, kGroundSize, 160, 160, 32.0f);

  _groundCmd.mesh = _terrainMesh.get();
  _groundCmd.shader = _terrainShader.get();
  _groundCmd.textures.push_back({_groundTexture.get(), "u_Texture", 0});
  _groundCmd.state = RenderState::DepthTest | RenderState::DepthWrite;
  _groundCmd.model = glm::mat4(1.0f);

  const glm::vec3 playerStart(0.0f, 0.0f, 25.0f);
  _playerBody = _physicsWorld.CreatePlayerCapsule(
      glm::vec3(playerStart.x,
                GetTerrainHeight(playerStart.x, playerStart.z) +
                    kPlayerStandingCenterY,
                playerStart.z),
      kPlayerCapsuleHalfHeight, kPlayerCapsuleRadius);

  _modelData = GltfLoader::Load("assets/models/enemy.glb");
  _modelShader = std::make_unique<Shader>("assets/shaders/skinned.vert",
                                          "assets/shaders/common.frag");

  if (!_modelData.animations.empty()) {
    _animator.SetAnimation(
        _modelData.animations[_modelData.animations.size() - 1].get(),
        _modelData.inverseBindMatrices, _modelData.boneParents,
        _modelData.boneNames, _modelData.boneBindTranslations,
        _modelData.boneBindRotations, _modelData.boneBindScales,
        _modelData.boneRootParentTransforms, true);
  }

  _modelCmd.mesh = _modelData.mesh.get();
  _modelCmd.shader = _modelShader.get();
  if (_animator.IsPlaying()) {
    _modelCmd.skinning.boneMatrices = &_animator.GetBoneMatrices();
  }
  if (_modelData.defaultMaterialIndex >= 0 &&
      _modelData.defaultMaterialIndex < _modelData.materials.size()) {
    _modelCmd.textures.push_back(
        {_modelData.materials[_modelData.defaultMaterialIndex]
             .albedoTexture.get(),
         "u_Texture", 0});
  }

  _modelCmd.model = EnemyOrbitTransform(_enemyOrbitAngle);

  const std::array<std::string, 6> skyboxFaces = {
      "assets/textures/skybox/space_rt.png",
      "assets/textures/skybox/space_lf.png",
      "assets/textures/skybox/space_up.png",
      "assets/textures/skybox/space_dn.png",
      "assets/textures/skybox/space_bk.png",
      "assets/textures/skybox/space_ft.png"};
  _skyboxTexture = std::make_shared<CubeMapTexture>(skyboxFaces);
  _skyboxShader = std::make_shared<Shader>("assets/shaders/skybox.vert",
                                           "assets/shaders/skybox.frag");
  _skyboxMesh = Sky::CreateSkyBoxMesh();

  _skyboxCmd.mesh = _skyboxMesh.get();
  _skyboxCmd.shader = _skyboxShader.get();
  _skyboxCmd.state = static_cast<RenderStateMask>(RenderState::DepthTest);
  _skyboxCmd.depthFunc = DepthFunc::LessEqual;
  _skyboxCmd.model = glm::mat4(1.0f);
  _skyboxCmd.textures.push_back({_skyboxTexture.get(), "u_Skybox", 0});

  _groundCmd.textures.push_back({_skyboxTexture.get(), "u_FogSkybox", 1});
  _modelCmd.textures.push_back({_skyboxTexture.get(), "u_FogSkybox", 1});
}

Level::~Level() {
  _physicsWorld.DestroyBody(_playerBody);
}

void Level::Update(const InputManager &input, Camera &camera, float dt) {
  if (input.IsKeyJustPressed(GLFW_KEY_F2)) {
    _freeCameraMode = !_freeCameraMode;
    if (!_freeCameraMode) {
      SyncPlayerToCamera(camera);
    }
  }

  if (_freeCameraMode) {
    camera.Update(input, dt);
  } else {
    camera.UpdateLook(input);
    UpdatePlayer(input, camera);
    _physicsWorld.Step(dt);
    ConstrainPlayerToTerrain();
    camera.SetPosition(GetPlayerEyePosition());
  }

  _animator.Update(dt);

  _enemyOrbitAngle += dt * kEnemyOrbitSpeed;
  _modelCmd.model = EnemyOrbitTransform(_enemyOrbitAngle);
}

void Level::Render() const {
  Renderer::Submit(_skyboxCmd);
  Renderer::Submit(_groundCmd);
  Renderer::Submit(_modelCmd);
}

void Level::UpdatePlayer(const InputManager &input, const Camera &camera) {
  JPH::BodyInterface &bodyInterface = _physicsWorld.GetBodyInterface();
  const JPH::RVec3 position = bodyInterface.GetCenterOfMassPosition(_playerBody);
  const JPH::Vec3 currentVelocity = bodyInterface.GetLinearVelocity(_playerBody);

  glm::vec3 forward(camera.GetFront().x, 0.0f, camera.GetFront().z);
  if (glm::dot(forward, forward) > 0.0001f)
    forward = glm::normalize(forward);

  glm::vec3 right(camera.GetRight().x, 0.0f, camera.GetRight().z);
  if (glm::dot(right, right) > 0.0001f)
    right = glm::normalize(right);

  glm::vec3 wishDir(0.0f);
  if (input.IsKeyPressed(GLFW_KEY_W))
    wishDir += forward;
  if (input.IsKeyPressed(GLFW_KEY_S))
    wishDir -= forward;
  if (input.IsKeyPressed(GLFW_KEY_D))
    wishDir += right;
  if (input.IsKeyPressed(GLFW_KEY_A))
    wishDir -= right;

  if (glm::dot(wishDir, wishDir) > 0.0001f)
    wishDir = glm::normalize(wishDir);

  const float groundY = GetTerrainHeight(static_cast<float>(position.GetX()),
                                         static_cast<float>(position.GetZ()));
  const float playerGroundCenterY = groundY + kPlayerStandingCenterY;
  const bool closeToGround = position.GetY() <=
                             playerGroundCenterY + kGroundedTolerance;
  _playerGrounded = closeToGround && currentVelocity.GetY() <= 0.5f;

  float verticalVelocity = currentVelocity.GetY();
  if (_playerGrounded && input.IsKeyJustPressed(GLFW_KEY_SPACE)) {
    verticalVelocity = kPlayerJumpSpeed;
    _playerGrounded = false;
  } else if (_playerGrounded) {
    verticalVelocity = 0.0f;
  }

  const glm::vec3 horizontalVelocity = wishDir * kPlayerMoveSpeed;
  bodyInterface.SetLinearVelocity(
      _playerBody, JPH::Vec3(horizontalVelocity.x, verticalVelocity,
                             horizontalVelocity.z));
  bodyInterface.ActivateBody(_playerBody);
}

void Level::ConstrainPlayerToTerrain() {
  JPH::BodyInterface &bodyInterface = _physicsWorld.GetBodyInterface();
  const JPH::RVec3 position = bodyInterface.GetCenterOfMassPosition(_playerBody);
  const JPH::Vec3 velocity = bodyInterface.GetLinearVelocity(_playerBody);

  const float groundY = GetTerrainHeight(static_cast<float>(position.GetX()),
                                         static_cast<float>(position.GetZ()));
  const float playerGroundCenterY = groundY + kPlayerStandingCenterY;
  const bool shouldSnapToGround =
      position.GetY() <= playerGroundCenterY || _playerGrounded;

  if (!shouldSnapToGround)
    return;

  bodyInterface.SetPositionAndRotation(
      _playerBody,
      JPH::RVec3(position.GetX(), playerGroundCenterY, position.GetZ()),
      JPH::Quat::sIdentity(), JPH::EActivation::Activate);
  bodyInterface.SetLinearVelocity(
      _playerBody, JPH::Vec3(velocity.GetX(), std::max(0.0f, velocity.GetY()),
                             velocity.GetZ()));
  _playerGrounded = true;
}

void Level::SyncPlayerToCamera(const Camera &camera) {
  glm::vec3 playerPosition = camera.GetPosition();
  const float playerGroundCenterY =
      GetTerrainHeight(playerPosition.x, playerPosition.z) +
      kPlayerStandingCenterY;
  playerPosition.y =
      std::max(playerPosition.y - kPlayerEyeOffsetY, playerGroundCenterY);

  JPH::BodyInterface &bodyInterface = _physicsWorld.GetBodyInterface();
  bodyInterface.SetPositionAndRotation(
      _playerBody,
      JPH::RVec3(playerPosition.x, playerPosition.y, playerPosition.z),
      JPH::Quat::sIdentity(), JPH::EActivation::Activate);
  bodyInterface.SetLinearVelocity(_playerBody, JPH::Vec3::sZero());
  _playerGrounded = false;
}

float Level::GetTerrainHeight(float x, float z) const {
  return SampleTerrainHeight(x, z, kGroundSize, kGroundSize);
}

glm::vec3 Level::GetPlayerEyePosition() {
  const JPH::RVec3 position =
      _physicsWorld.GetBodyInterface().GetCenterOfMassPosition(_playerBody);
  return glm::vec3(static_cast<float>(position.GetX()),
                   static_cast<float>(position.GetY()) + kPlayerEyeOffsetY,
                   static_cast<float>(position.GetZ()));
}
