#pragma once

#include "PhysicsWorld.h"
#include <Jolt/Physics/Body/BodyID.h>
#include <glm/glm.hpp>

class Camera;
class InputManager;

class PlayerController {
public:
  void Spawn(PhysicsWorld &physicsWorld, const glm::vec3 &startPosition,
             float terrainWidth, float terrainDepth);
  void Destroy(PhysicsWorld &physicsWorld);

  void Update(const InputManager &input, const Camera &camera,
              PhysicsWorld &physicsWorld);
  void ConstrainToTerrain(PhysicsWorld &physicsWorld);
  void SyncToCamera(const Camera &camera, PhysicsWorld &physicsWorld);
  glm::vec3 GetEyePosition(PhysicsWorld &physicsWorld) const;

private:
  JPH::BodyID _body;
  bool _grounded = false;
  float _terrainWidth = 1.0f;
  float _terrainDepth = 1.0f;

  float GetTerrainHeight(float x, float z) const;
};
