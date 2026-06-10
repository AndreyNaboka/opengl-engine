#include "Level.h"
#include "Sky.h"
#include "TerrainGenerator.h"
#include <array>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

Level::Level() {
  _terrainShader = std::make_unique<Shader>("assets/shaders/terrain.vert",
                                            "assets/shaders/common.frag");
  _groundTexture = std::make_unique<Texture>("assets/textures/grass.png");
  _terrainMesh = GenerateGrid(200.0f, 200.0f, 60, 60, 15.0f);

  _groundCmd.mesh = _terrainMesh.get();
  _groundCmd.shader = _terrainShader.get();
  _groundCmd.textures.push_back({_groundTexture.get(), "u_Texture", 0});
  _groundCmd.state = RenderState::DepthTest | RenderState::DepthWrite;
  _groundCmd.model = glm::mat4(1.0f);

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

  const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(6.0f));
  const glm::mat4 translate =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -5.0f));
  _modelCmd.model = translate * scale;

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
}

void Level::Update(float dt) { _animator.Update(dt); }

void Level::Render() const {
  Renderer::Submit(_skyboxCmd);
  Renderer::Submit(_groundCmd);
  Renderer::Submit(_modelCmd);
}
