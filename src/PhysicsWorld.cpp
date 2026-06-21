#include "PhysicsWorld.h"
#include "Utils/Logger.h"
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/AllowedDOFs.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <thread>

namespace {
constexpr float kFixedPhysicsStep = 1.0f / 60.0f;
constexpr float kMaxFrameStep = 1.0f / 15.0f;

#if defined(__GNUC__) || defined(__clang__)
void TraceImpl(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));
#endif

void TraceImpl(const char *fmt, ...) {
  char buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  LogInfo("[Jolt] ", buffer);
}

#ifdef JPH_ENABLE_ASSERTS
bool AssertFailedImpl(const char *expression, const char *message,
                      const char *file, JPH::uint line) {
  LogInfo("[Jolt] Assertion failed ", file, ":", line, " (", expression, ") ",
          message ? message : "");
  return true;
}
#endif

namespace BroadPhaseLayers {
constexpr JPH::BroadPhaseLayer NonMoving(0);
constexpr JPH::BroadPhaseLayer Moving(1);
constexpr JPH::uint Count = 2;
} // namespace BroadPhaseLayers

std::mutex g_joltLifecycleMutex;
int g_joltWorldCount = 0;

void AcquireJoltRuntime() {
  std::lock_guard lock(g_joltLifecycleMutex);
  if (g_joltWorldCount++ > 0)
    return;

  JPH::RegisterDefaultAllocator();
  JPH::Trace = TraceImpl;
  JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

  JPH::Factory::sInstance = new JPH::Factory();
  JPH::RegisterTypes();
}

void ReleaseJoltRuntime() {
  std::lock_guard lock(g_joltLifecycleMutex);
  if (--g_joltWorldCount > 0)
    return;

  JPH::UnregisterTypes();
  delete JPH::Factory::sInstance;
  JPH::Factory::sInstance = nullptr;
}
} // namespace

class PhysicsWorld::BroadPhaseLayerInterfaceImpl final
    : public JPH::BroadPhaseLayerInterface {
public:
  BroadPhaseLayerInterfaceImpl() {
    _objectToBroadPhase[PhysicsLayers::NonMoving] = BroadPhaseLayers::NonMoving;
    _objectToBroadPhase[PhysicsLayers::Moving] = BroadPhaseLayers::Moving;
  }

  JPH::uint GetNumBroadPhaseLayers() const override {
    return BroadPhaseLayers::Count;
  }

  JPH::BroadPhaseLayer
  GetBroadPhaseLayer(JPH::ObjectLayer layer) const override {
    JPH_ASSERT(layer < PhysicsLayers::Count);
    return _objectToBroadPhase[layer];
  }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
  const char *
  GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override {
    switch (static_cast<JPH::BroadPhaseLayer::Type>(layer)) {
    case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::NonMoving):
      return "NonMoving";
    case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::Moving):
      return "Moving";
    default:
      return "Invalid";
    }
  }
#endif

private:
  JPH::BroadPhaseLayer _objectToBroadPhase[PhysicsLayers::Count];
};

class PhysicsWorld::ObjectVsBroadPhaseLayerFilterImpl final
    : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
  bool ShouldCollide(JPH::ObjectLayer layer,
                     JPH::BroadPhaseLayer broadPhaseLayer) const override {
    switch (layer) {
    case PhysicsLayers::NonMoving:
      return broadPhaseLayer == BroadPhaseLayers::Moving;
    case PhysicsLayers::Moving:
      return true;
    default:
      JPH_ASSERT(false);
      return false;
    }
  }
};

class PhysicsWorld::ObjectLayerPairFilterImpl final
    : public JPH::ObjectLayerPairFilter {
public:
  bool ShouldCollide(JPH::ObjectLayer object1,
                     JPH::ObjectLayer object2) const override {
    switch (object1) {
    case PhysicsLayers::NonMoving:
      return object2 == PhysicsLayers::Moving;
    case PhysicsLayers::Moving:
      return true;
    default:
      JPH_ASSERT(false);
      return false;
    }
  }
};

class PhysicsWorld::RuntimeHandle final {
public:
  RuntimeHandle() { AcquireJoltRuntime(); }
  ~RuntimeHandle() { ReleaseJoltRuntime(); }
};

PhysicsWorld::PhysicsWorld()
    : _runtime(std::make_unique<RuntimeHandle>()),
      _broadPhaseLayerInterface(
          std::make_unique<BroadPhaseLayerInterfaceImpl>()),
      _objectVsBroadPhaseLayerFilter(
          std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>()),
      _objectLayerPairFilter(std::make_unique<ObjectLayerPairFilterImpl>()) {
  _tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
  _jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
      JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
      std::max(1u, std::thread::hardware_concurrency()));

  constexpr JPH::uint maxBodies = 1024;
  constexpr JPH::uint numBodyMutexes = 0;
  constexpr JPH::uint maxBodyPairs = 1024;
  constexpr JPH::uint maxContactConstraints = 1024;
  _physicsSystem.Init(maxBodies, numBodyMutexes, maxBodyPairs,
                      maxContactConstraints, *_broadPhaseLayerInterface,
                      *_objectVsBroadPhaseLayerFilter, *_objectLayerPairFilter);
  _physicsSystem.SetGravity(JPH::Vec3(0.0f, -24.0f, 0.0f));
}

PhysicsWorld::~PhysicsWorld() = default;

JPH::BodyID PhysicsWorld::CreateStaticBox(const glm::vec3 &position,
                                          const glm::vec3 &halfExtent) {
  JPH::BodyCreationSettings settings(
      new JPH::BoxShape(JPH::Vec3(halfExtent.x, halfExtent.y, halfExtent.z)),
      JPH::RVec3(position.x, position.y, position.z), JPH::Quat::sIdentity(),
      JPH::EMotionType::Static, PhysicsLayers::NonMoving);
  settings.mFriction = 0.85f;

  JPH::BodyInterface &bodyInterface = _physicsSystem.GetBodyInterface();
  return bodyInterface.CreateAndAddBody(settings,
                                        JPH::EActivation::DontActivate);
}

JPH::BodyID PhysicsWorld::CreatePlayerCapsule(const glm::vec3 &position,
                                              float halfHeight, float radius) {
  JPH::BodyCreationSettings settings(
      new JPH::CapsuleShape(halfHeight, radius),
      JPH::RVec3(position.x, position.y, position.z), JPH::Quat::sIdentity(),
      JPH::EMotionType::Dynamic, PhysicsLayers::Moving);
  settings.mAllowedDOFs = JPH::EAllowedDOFs::TranslationX |
                          JPH::EAllowedDOFs::TranslationY |
                          JPH::EAllowedDOFs::TranslationZ;
  settings.mAllowSleeping = false;
  settings.mFriction = 0.0f;
  settings.mLinearDamping = 0.0f;
  settings.mMotionQuality = JPH::EMotionQuality::LinearCast;

  JPH::BodyInterface &bodyInterface = _physicsSystem.GetBodyInterface();
  return bodyInterface.CreateAndAddBody(settings, JPH::EActivation::Activate);
}

void PhysicsWorld::DestroyBody(JPH::BodyID bodyId) {
  if (bodyId.IsInvalid())
    return;

  JPH::BodyInterface &bodyInterface = _physicsSystem.GetBodyInterface();
  bodyInterface.RemoveBody(bodyId);
  bodyInterface.DestroyBody(bodyId);
}

void PhysicsWorld::Step(float dt) {
  const float clampedDt = std::clamp(dt, 0.0f, kMaxFrameStep);
  const int collisionSteps =
      std::max(1, static_cast<int>(std::ceil(clampedDt / kFixedPhysicsStep)));
  _physicsSystem.Update(clampedDt, collisionSteps, _tempAllocator.get(),
                        _jobSystem.get());
}

JPH::BodyInterface &PhysicsWorld::GetBodyInterface() {
  return _physicsSystem.GetBodyInterface();
}
