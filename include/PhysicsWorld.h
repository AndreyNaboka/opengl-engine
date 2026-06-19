#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <glm/glm.hpp>
#include <memory>

namespace PhysicsLayers {
constexpr JPH::ObjectLayer NonMoving = 0;
constexpr JPH::ObjectLayer Moving = 1;
constexpr JPH::ObjectLayer Count = 2;
} // namespace PhysicsLayers

class PhysicsWorld {
public:
  PhysicsWorld();
  ~PhysicsWorld();

  PhysicsWorld(const PhysicsWorld &) = delete;
  PhysicsWorld &operator=(const PhysicsWorld &) = delete;

  JPH::BodyID CreateStaticBox(const glm::vec3 &position,
                              const glm::vec3 &halfExtent);
  JPH::BodyID CreatePlayerCapsule(const glm::vec3 &position, float halfHeight,
                                  float radius);

  void DestroyBody(JPH::BodyID bodyId);
  void Step(float dt);

  JPH::BodyInterface &GetBodyInterface();

private:
  class BroadPhaseLayerInterfaceImpl;
  class ObjectVsBroadPhaseLayerFilterImpl;
  class ObjectLayerPairFilterImpl;
  class RuntimeHandle;

  std::unique_ptr<RuntimeHandle> _runtime;
  std::unique_ptr<BroadPhaseLayerInterfaceImpl> _broadPhaseLayerInterface;
  std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl>
      _objectVsBroadPhaseLayerFilter;
  std::unique_ptr<ObjectLayerPairFilterImpl> _objectLayerPairFilter;

  JPH::PhysicsSystem _physicsSystem;
  std::unique_ptr<JPH::TempAllocatorImpl> _tempAllocator;
  std::unique_ptr<JPH::JobSystemThreadPool> _jobSystem;
};
