#include "GltfLoader.h"
#include "Utils/PathUtils.h"
#include "Utils/Logger.h"
#include <cgltf.h>
#include <stdexcept>
#include <numeric>
#include <cstdint>
#include <cstring>

static const char *GltfResultToString(cgltf_result res) {
  switch (res) {
  case cgltf_result_success:
    return "Success";
  case cgltf_result_data_too_short:
    return "Data too short";
  case cgltf_result_unknown_format:
    return "Unknown format";
  case cgltf_result_invalid_json:
    return "Invalid JSON";
  case cgltf_result_invalid_gltf:
    return "Invalid glTF";
  case cgltf_result_invalid_options:
    return "Invalid options";
  case cgltf_result_file_not_found:
    return "File not found";
  case cgltf_result_io_error:
    return "I/O error";
  case cgltf_result_out_of_memory:
    return "Out of memory";
  case cgltf_result_legacy_gltf:
    return "Legacy glTF v1.0";
  default:
    return "Unknown error";
  }
}

GltfModelData GltfLoader::Load(const std::string &assetPath) {
  GltfModelData result;
  auto absPath = Path::ResolveAssetPath(assetPath);

  LogInfo("[GltfLoader] try to load model " + absPath.string());

  cgltf_options opts = {};
  cgltf_data *data = nullptr;

  cgltf_result res = cgltf_parse_file(&opts, absPath.string().c_str(), &data);
  if (res != cgltf_result_success)
    LogInfo("[GltfLoader] " + std::string("cgltf_parse failed: ") +
            GltfResultToString(res));

  res = cgltf_load_buffers(&opts, data, absPath.string().c_str());
  if (res != cgltf_result_success) {
    cgltf_free(data);
    LogInfo("[GltfLoader] " + std::string("cgltf_load_buffers failed: ") +
            GltfResultToString(res));
  }

  if (data->meshes_count == 0) {
    cgltf_free(data);
    return result;
  }

  const cgltf_mesh *targetMesh = &data->meshes[0];
  cgltf_skin *activeSkin = nullptr;
  bool isSkinned = false;

  for (size_t n = 0; n < data->nodes_count; ++n) {
    if (data->nodes[n].mesh == targetMesh && data->nodes[n].skin != nullptr) {
      isSkinned = true;
      activeSkin = data->nodes[n].skin;
      break;
    }
  }

  const cgltf_primitive &prim = targetMesh->primitives[0];

  const cgltf_accessor *posAcc = nullptr, *normAcc = nullptr, *uvAcc = nullptr;
  const cgltf_accessor *jointsAcc = nullptr, *weightsAcc = nullptr;
  for (size_t i = 0; i < prim.attributes_count; ++i) {
    const auto &attr = prim.attributes[i];
    if (strcmp(attr.name, "POSITION") == 0)
      posAcc = attr.data;
    else if (strcmp(attr.name, "NORMAL") == 0)
      normAcc = attr.data;
    else if (strcmp(attr.name, "TEXCOORD_0") == 0)
      uvAcc = attr.data;
    else if (strcmp(attr.name, "JOINTS_0") == 0)
      jointsAcc = attr.data;
    else if (strcmp(attr.name, "WEIGHTS_0") == 0)
      weightsAcc = attr.data;
  }
  if (!posAcc) {
    cgltf_free(data);
    throw std::runtime_error("Missing POSITION attribute");
  }

  size_t vCount = posAcc->count;
  std::vector<Vertex> vertices(
      vCount); // Vertex() = default инициализирует кости в -1/0

  cgltf_accessor_read_float(
      posAcc, 0, reinterpret_cast<float *>(&vertices[0].position), vCount * 3);
  if (normAcc)
    cgltf_accessor_read_float(
        normAcc, 0, reinterpret_cast<float *>(&vertices[0].normal), vCount * 3);

  if (uvAcc) {
    std::vector<float> tempUV(vCount * 2);
    cgltf_accessor_read_float(uvAcc, 0, tempUV.data(), vCount * 2);
    for (size_t i = 0; i < vCount; ++i)
      vertices[i].uv = {tempUV[i * 2], tempUV[i * 2 + 1]};
  }

  if (isSkinned) {
    if (!jointsAcc || !weightsAcc)
      LogInfo("[GltfLoader] Skinned mesh missing JOINTS_0/WEIGHTS_0");

    cgltf_accessor_read_float(
        weightsAcc, 0, reinterpret_cast<float *>(&vertices[0].boneWeights[0]),
        vCount * 4);

    cgltf_buffer_view *jView = jointsAcc->buffer_view;
    const uint8_t *jData = static_cast<const uint8_t *>(jView->buffer->data) +
                           jView->offset + jointsAcc->offset;
    size_t jStride = jView->stride > 0
                         ? jView->stride
                         : cgltf_component_size(jointsAcc->component_type) * 4;
    bool isUShort = (jointsAcc->component_type == cgltf_component_type_r_16u);

    for (size_t i = 0; i < vCount; ++i) {
      const uint8_t *ptr = jData + i * jStride;
      if (isUShort) {
        const uint16_t *u16 = reinterpret_cast<const uint16_t *>(ptr);
        for (int j = 0; j < 4; ++j)
          vertices[i].boneIds[j] = static_cast<int>(u16[j]);
      } else {
        for (int j = 0; j < 4; ++j)
          vertices[i].boneIds[j] = static_cast<int>(ptr[j]);
      }
    }
  }

  std::vector<uint32_t> indices;
  if (prim.indices) {
    size_t iCount = prim.indices->count;
    indices.resize(iCount);
    cgltf_accessor_read_uint(prim.indices, 0, indices.data(), iCount);
  } else {
    indices.resize(vCount);
    std::iota(indices.begin(), indices.end(), 0);
  }

  result.mesh = std::make_shared<Mesh>(vertices, indices);
  result.isSkinned = isSkinned;

  if (isSkinned && activeSkin) {
    size_t jointCount = activeSkin->joints_count;
    result.inverseBindMatrices.resize(jointCount, glm::mat4(1.0f));
    if (activeSkin->inverse_bind_matrices) {
      cgltf_accessor_read_float(
          activeSkin->inverse_bind_matrices, 0,
          reinterpret_cast<float *>(result.inverseBindMatrices.data()),
          jointCount * 16);
    }
    for (size_t i = 0; i < jointCount; ++i) {
      const cgltf_node *jn = activeSkin->joints[i];
      std::string name = jn->name ? jn->name : ("joint_" + std::to_string(i));
      result.boneNameToIndex[name] = i;
    }
  }
  result.animations.reserve(data->animations_count);
  for (size_t a = 0; a < data->animations_count; ++a) {
    auto anim = std::make_shared<Animation>();
    const cgltf_animation &cgAnim = data->animations[a];
    float maxTime = 0.0f;

    for (size_t c = 0; c < cgAnim.channels_count; ++c) {
      const cgltf_animation_channel &ch = cgAnim.channels[c];

      if (!ch.target_node || !ch.sampler)
        continue;

      const cgltf_animation_sampler &sampler = *ch.sampler;
      if (!sampler.input || !sampler.output)
        continue;

      size_t keyCount = sampler.input->count;
      if (keyCount > 0) {
        float lastTime = 0.0f;
        cgltf_accessor_read_float(sampler.input, keyCount - 1, &lastTime, 1);
        if (lastTime > maxTime)
          maxTime = lastTime;
      }

      std::string boneName =
          ch.target_node->name
              ? ch.target_node->name
              : ("node_" + std::to_string(ch.target_node - data->nodes));

      size_t boneIdx = 0;
      auto it = result.boneNameToIndex.find(boneName);
      if (it != result.boneNameToIndex.end())
        boneIdx = it->second;
      else {
        boneIdx = result.boneNameToIndex.size();
        result.boneNameToIndex[boneName] = boneIdx;
      }

      BoneChannel *targetCh = nullptr;
      for (auto &bc : anim->channels)
        if (bc.boneName == boneName) {
          targetCh = &bc;
          break;
        }
      if (!targetCh) {
        anim->channels.push_back({boneName, {}});
        targetCh = &anim->channels.back();
      }

      std::vector<float> times(keyCount);
      cgltf_accessor_read_float(sampler.input, 0, times.data(), keyCount);

      bool isQuat = (ch.target_path == cgltf_animation_path_type_rotation);
      int compCount = isQuat ? 4 : 3;
      std::vector<float> values(keyCount * compCount);
      cgltf_accessor_read_float(sampler.output, 0, values.data(),
                                keyCount * compCount);

      for (size_t k = 0; k < keyCount; ++k) {
        float time = times[k];
        Keyframe *kf = nullptr;
        for (auto &existing : targetCh->keyframes)
          if (std::abs(existing.time - time) < 1e-5f) {
            kf = &existing;
            break;
          }
        if (!kf) {
          targetCh->keyframes.push_back({time});
          kf = &targetCh->keyframes.back();
        }

        if (isQuat) {
          kf->rotation = {values[k * 4], values[k * 4 + 1], values[k * 4 + 2],
                          values[k * 4 + 3]};
          kf->hasRotation = true;
        } else if (ch.target_path == cgltf_animation_path_type_translation) {
          kf->position = {values[k * 3], values[k * 3 + 1], values[k * 3 + 2]};
          kf->hasPosition = true;
        } else if (ch.target_path == cgltf_animation_path_type_scale) {
          kf->scale = {values[k * 3], values[k * 3 + 1], values[k * 3 + 2]};
          kf->hasScale = true;
        }
      }
    }
    anim->duration = maxTime;
    result.animations.push_back(anim);
  }

  cgltf_free(data);
  return result;
}
