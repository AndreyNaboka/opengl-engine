#include "PlayerController.h"
#include "Camera.h"
#include "TerrainGenerator.h"
#include "InputManager.h"
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/EActivation.h>
#include <GLFW/glfw3.h>
#include <algorithm>

namespace {
constexpr float kCapsuleHalfHeight = 0.9f;
constexpr float kCapsuleRadius = 0.35f;
constexpr float kStandingCenterY = kCapsuleHalfHeight + kCapsuleRadius;
constexpr float kEyeOffsetY = 7.45f;
constexpr float kMoveSpeed = 10.0f;
constexpr float kJumpSpeed = 10.5f;
constexpr float kGroundedTolerance = 0.08f;
} // namespace

void PlayerController::Spawn(PhysicsWorld &physicsWorld,
                             const glm::vec3 &startPosition,
                             float terrainWidth, float terrainDepth) {
  _terrainWidth = terrainWidth;
  _terrainDepth = terrainDepth;
  _grounded = false;

  _body = physicsWorld.CreatePlayerCapsule(
      glm::vec3(startPosition.x,
                GetTerrainHeight(startPosition.x, startPosition.z) +
                    kStandingCenterY,
                startPosition.z),
      kCapsuleHalfHeight, kCapsuleRadius);
}

void PlayerController::Destroy(PhysicsWorld &physicsWorld) {
  physicsWorld.DestroyBody(_body);
  _body = JPH::BodyID();
  _grounded = false;
}

void PlayerController::Update(const InputManager &input, const Camera &camera,
                              PhysicsWorld &physicsWorld) {
  JPH::BodyInterface &bodyInterface = physicsWorld.GetBodyInterface();
  const JPH::RVec3 position = bodyInterface.GetCenterOfMassPosition(_body);
  const JPH::Vec3 currentVelocity = bodyInterface.GetLinearVelocity(_body);

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
  const float playerGroundCenterY = groundY + kStandingCenterY;
  const bool closeToGround =
      position.GetY() <= playerGroundCenterY + kGroundedTolerance;
  _grounded = closeToGround && currentVelocity.GetY() <= 0.5f;

  float verticalVelocity = currentVelocity.GetY();
  if (_grounded && input.IsKeyJustPressed(GLFW_KEY_SPACE)) {
    verticalVelocity = kJumpSpeed;
    _grounded = false;
  } else if (_grounded) {
    verticalVelocity = 0.0f;
  }

  const glm::vec3 horizontalVelocity = wishDir * kMoveSpeed;
  bodyInterface.SetLinearVelocity(
      _body, JPH::Vec3(horizontalVelocity.x, verticalVelocity,
                       horizontalVelocity.z));
  bodyInterface.ActivateBody(_body);
}

void PlayerController::ConstrainToTerrain(PhysicsWorld &physicsWorld) {
  JPH::BodyInterface &bodyInterface = physicsWorld.GetBodyInterface();
  const JPH::RVec3 position = bodyInterface.GetCenterOfMassPosition(_body);
  const JPH::Vec3 velocity = bodyInterface.GetLinearVelocity(_body);

  const float groundY = GetTerrainHeight(static_cast<float>(position.GetX()),
                                         static_cast<float>(position.GetZ()));
  const float playerGroundCenterY = groundY + kStandingCenterY;
  const bool shouldSnapToGround =
      position.GetY() <= playerGroundCenterY || _grounded;

  if (!shouldSnapToGround)
    return;

  bodyInterface.SetPositionAndRotation(
      _body, JPH::RVec3(position.GetX(), playerGroundCenterY, position.GetZ()),
      JPH::Quat::sIdentity(), JPH::EActivation::Activate);
  bodyInterface.SetLinearVelocity(
      _body, JPH::Vec3(velocity.GetX(), std::max(0.0f, velocity.GetY()),
                       velocity.GetZ()));
  _grounded = true;
}

void PlayerController::SyncToCamera(const Camera &camera,
                                    PhysicsWorld &physicsWorld) {
  glm::vec3 playerPosition = camera.GetPosition();
  const float playerGroundCenterY =
      GetTerrainHeight(playerPosition.x, playerPosition.z) + kStandingCenterY;
  playerPosition.y =
      std::max(playerPosition.y - kEyeOffsetY, playerGroundCenterY);

  JPH::BodyInterface &bodyInterface = physicsWorld.GetBodyInterface();
  bodyInterface.SetPositionAndRotation(
      _body, JPH::RVec3(playerPosition.x, playerPosition.y, playerPosition.z),
      JPH::Quat::sIdentity(), JPH::EActivation::Activate);
  bodyInterface.SetLinearVelocity(_body, JPH::Vec3::sZero());
  _grounded = false;
}

glm::vec3 PlayerController::GetEyePosition(PhysicsWorld &physicsWorld) const {
  const JPH::RVec3 position =
      physicsWorld.GetBodyInterface().GetCenterOfMassPosition(_body);
  return glm::vec3(static_cast<float>(position.GetX()),
                   static_cast<float>(position.GetY()) + kEyeOffsetY,
                   static_cast<float>(position.GetZ()));
}

float PlayerController::GetTerrainHeight(float x, float z) const {
  return SampleTerrainHeight(x, z, _terrainWidth, _terrainDepth);
}
