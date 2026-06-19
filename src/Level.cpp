#include "Level.h"
#include "Camera.h"
#include "InputManager.h"
#include "Sky.h"
#include "TerrainGenerator.h"
#include <array>
#include <cmath>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

namespace {
constexpr float kEnemyScale = 6.0f;
constexpr float kEnemyGroundOffsetY = 2.0f;
constexpr float kEnemyOrbitRadius = 18.0f;
constexpr float kEnemyOrbitSpeed = 0.65f;
constexpr float kGroundSize = 420.0f;
constexpr glm::vec3 kSunDirection(-0.35f, -0.82f, -0.45f);
constexpr glm::vec3 kSunColor(1.0f, 0.86f, 0.62f);
constexpr float kSunAmbientStrength = 0.16f;

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
  Renderer::SetSunLight(kSunDirection, kSunColor, kSunAmbientStrength);
  SetupTerrain();
  SetupPlayer();
  SetupModel();
  SetupSkybox();
}

void Level::SetupTerrain() {
  _terrainShader = std::make_unique<Shader>("assets/shaders/terrain.vert",
                                            "assets/shaders/common.frag");
  _groundTexture = std::make_unique<Texture>("assets/textures/grass.png");
  _terrainMesh = GenerateGrid(kGroundSize, kGroundSize, 160, 160, 32.0f);

  _groundCmd.mesh = _terrainMesh.get();
  _groundCmd.shader = _terrainShader.get();
  _groundCmd.textures.push_back({_groundTexture.get(), "u_Texture", 0});
  _groundCmd.state = RenderState::DepthTest | RenderState::DepthWrite;
  _groundCmd.model = glm::mat4(1.0f);
}

void Level::SetupPlayer() {
  const glm::vec3 playerStart(0.0f, 0.0f, 25.0f);
  _playerController.Spawn(_physicsWorld, playerStart, kGroundSize, kGroundSize);
}

void Level::SetupModel() {
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
}

void Level::SetupSkybox() {
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
  _playerController.Destroy(_physicsWorld);
}

void Level::Update(const InputManager &input, Camera &camera, float dt) {
  if (camera.UpdateFreeCameraMode(input) && !camera.IsFreeCameraMode()) {
    _playerController.SyncToCamera(camera, _physicsWorld);
  }

  if (camera.IsFreeCameraMode()) {
    camera.Update(input, dt);
  } else {
    camera.UpdateLook(input);
    _playerController.Update(input, camera, _physicsWorld);
    _physicsWorld.Step(dt);
    _playerController.ConstrainToTerrain(_physicsWorld);
    camera.SetPosition(_playerController.GetEyePosition(_physicsWorld));
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
