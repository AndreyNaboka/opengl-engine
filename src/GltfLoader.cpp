#include "GltfLoader.h"
#include "Utils/PathUtils.h"
#include "Utils/Logger.h"
#include <cgltf.h>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>
#include <memory>

static const char *GltfResultToString(cgltf_result res);

std::shared_ptr<Texture>
GltfLoader::LoadTextureFromCgltf(const cgltf_image *image,
                                 const std::string &basePath) {
  if (!image)
    return nullptr;

  std::shared_ptr<Texture> texture = nullptr;
  // inbound texture
  if (image->buffer_view) {
    const cgltf_buffer_view *view = image->buffer_view;
    const cgltf_buffer *buffer = view->buffer;
    if (buffer && buffer->data) {
      const uint8_t *data =
          static_cast<const uint8_t *>(buffer->data) + view->offset;
      size_t size = view->size;
      LogInfo("[GltfLoader] Found embedded texture, size: " +
              std::to_string(size) + " bytes, mime_type: " +
              (image->mime_type ? image->mime_type : "unknown"));
      texture = std::make_shared<Texture>(data, size, image->mime_type);
    }
  } else if (image->uri) {
    if (strncmp(image->uri, "data:", 5) == 0) {
      LogInfo("[GltfLoader] Data URI texture found (base64), skipping for now");
      // TODO Для base64 потребуется дополнительная обработка
    } else {
      std::string texturePath = basePath + "/" + image->uri;
      try {
        texture = std::make_shared<Texture>(texturePath);
        LogInfo("[GltfLoader] Loaded external texture: " + texturePath);
      } catch (const std::exception &e) {
        LogInfo("[GltfLoader] Failed to load texture: " + texturePath +
                ", error: " + e.what());
      }
    }
  }
  return texture;
}

GltfModelData::Material GltfLoader::LoadMaterial(const cgltf_material *material,
                                                 const std::string &basePath) {
  GltfModelData::Material result;
  if (!material)
    return result;
  // Get main texture
  if (material->pbr_metallic_roughness.base_color_texture.texture) {
    const cgltf_texture *tex =
        material->pbr_metallic_roughness.base_color_texture.texture;
    if (tex->image) {
      result.albedoTexture = LoadTextureFromCgltf(tex->image, basePath);
      LogInfo("[GltfLoader] Loaded albedo texture for material: " +
              std::string(material->name ? material->name : "unnamed"));
    }
  }
  // Get base color
  const float *color = material->pbr_metallic_roughness.base_color_factor;
  result.baseColor = glm::vec3(color[0], color[1], color[2]);
  result.metallic = material->pbr_metallic_roughness.metallic_factor;
  result.roughness = material->pbr_metallic_roughness.roughness_factor;
  return result;
}

GltfModelData GltfLoader::Load(const std::string &assetPath) {
  GltfModelData result;
  auto absPath = Path::ResolveAssetPath(assetPath);

  LogInfo("[GltfLoader] File: " + absPath.string());

  cgltf_options opts = {};
  cgltf_data *data = nullptr;

  cgltf_result res = cgltf_parse_file(&opts, absPath.string().c_str(), &data);
  if (res != cgltf_result_success) {
    LogInfo("[GltfLoader] FAILED: cgltf_parse error: " +
            std::string(GltfResultToString(res)));
    return result;
  }

  res = cgltf_load_buffers(&opts, data, absPath.string().c_str());
  if (res != cgltf_result_success) {
    LogInfo("[GltfLoader] FAILED: cgltf_load_buffers error: " +
            std::string(GltfResultToString(res)));
    cgltf_free(data);
    return result;
  }

  if (data->meshes_count == 0) {
    LogInfo("[GltfLoader] No meshes found");
    cgltf_free(data);
    return result;
  }

  // Load material
  const std::string basePath = absPath.parent_path().string();
  LogInfo("[GltfLoader] Loading " + std::to_string(data->materials_count) +
          " materials");
  result.materials.reserve(data->materials_count);
  for (size_t i = 0; i < data->materials_count; ++i) {
    GltfModelData::Material material =
        LoadMaterial(&data->materials[i], basePath);
    result.materials.push_back(material);
  }

  const cgltf_mesh *targetMesh = &data->meshes[0];
  const cgltf_primitive &prim = targetMesh->primitives[0];

  if (prim.material) {
    for (size_t i = 0; i < data->materials_count; ++i) {
      if (&data->materials[i] == prim.material) {
        result.defaultMaterialIndex = static_cast<int>(i);
        LogInfo("[GltfLoader] Primitive uses material index: " +
                std::to_string(i));
        break;
      }
    }
  }

  // Load skeleton
  cgltf_skin *activeSkin = nullptr;
  for (size_t n = 0; n < data->nodes_count; ++n) {
    if (data->nodes[n].mesh == targetMesh && data->nodes[n].skin) {
      activeSkin = data->nodes[n].skin;
      break;
    }
  }
  if (activeSkin) {
    LogInfo("[GltfLoader] Skin load success");
    result.isSkinned = true;
    const size_t jointCount = activeSkin->joints_count;
    result.inverseBindMatrices.resize(jointCount, glm::mat4(1.0f));
    result.boneBindTranslations.resize(jointCount, glm::vec3(0.0f));
    result.boneBindRotations.resize(jointCount,
                                    glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    result.boneBindScales.resize(jointCount, glm::vec3(1.0f));
    result.boneRootParentTransforms.resize(jointCount, glm::mat4(1.0f));

    if (activeSkin->inverse_bind_matrices) {
      for (size_t j = 0; j < jointCount; ++j) {
        cgltf_accessor_read_float(activeSkin->inverse_bind_matrices, j,
                                  glm::value_ptr(result.inverseBindMatrices[j]),
                                  16);
      }
    }
    result.boneNames.resize(jointCount);
    result.boneParents.resize(jointCount, -1);
    for (size_t j = 0; j < jointCount; ++j) {
      cgltf_node *node = activeSkin->joints[j];
      std::string name =
          node->name ? node->name : ("bone_" + std::to_string(j));
      result.boneNames[j] = name;
      result.boneNameToIndex[name] = j;
      if (node->has_translation) {
        result.boneBindTranslations[j] = glm::vec3(
            node->translation[0], node->translation[1], node->translation[2]);
      }
      if (node->has_rotation) {
        result.boneBindRotations[j] =
            glm::quat(node->rotation[3], node->rotation[0], node->rotation[1],
                      node->rotation[2]);
      }
      if (node->has_scale) {
        result.boneBindScales[j] =
            glm::vec3(node->scale[0], node->scale[1], node->scale[2]);
      }
      // find parent index
      if (node->parent) {
        bool parentIsJoint = false;
        for (size_t p = 0; p < jointCount; ++p) {
          if (activeSkin->joints[p] == node->parent) {
            result.boneParents[j] = static_cast<int>(p);
            parentIsJoint = true;
            break;
          }
        }
        if (!parentIsJoint) {
          float parentWorld[16];
          cgltf_node_transform_world(node->parent, parentWorld);
          result.boneRootParentTransforms[j] = glm::make_mat4(parentWorld);
        }
      }
    }
  }

  result.animations.reserve(data->animations_count);
  for (size_t a = 0; a < data->animations_count; ++a) {
    auto anim = std::make_shared<Animation>();
    const cgltf_animation &cgAnim = data->animations[a];
    anim->duration = 0.0f;

    // First pass: fill boneIndexMap from channels
    for (size_t c = 0; c < cgAnim.channels_count; ++c) {
      const cgltf_animation_channel &ch = cgAnim.channels[c];
      if (!ch.target_node || !ch.sampler)
        continue;
      std::string boneName = ch.target_node->name ? ch.target_node->name : "";
      auto it = result.boneNameToIndex.find(boneName);
      if (it != result.boneNameToIndex.end()) {
        anim->boneIndexMap[boneName] = it->second;
      }
    }

    // Second pass: read keyframes per channel
    for (size_t c = 0; c < cgAnim.channels_count; ++c) {
      const cgltf_animation_channel &ch = cgAnim.channels[c];
      if (!ch.target_node || !ch.sampler)
        continue;
      std::string boneName = ch.target_node->name ? ch.target_node->name : "";
      if (anim->boneIndexMap.find(boneName) == anim->boneIndexMap.end())
        continue;

      const cgltf_animation_sampler &sampler = *ch.sampler;
      if (!sampler.input || !sampler.output)
        continue;
      size_t keyCount = sampler.input->count;
      if (keyCount == 0)
        continue;
      std::vector<float> times(keyCount);
      for (size_t i = 0; i < keyCount; ++i)
        cgltf_accessor_read_float(sampler.input, i, &times[i], 1);
      if (!times.empty())
        anim->duration = std::max(anim->duration, times.back());
      // Find or create BoneChannel
      BoneChannel *boneChannel = nullptr;
      for (auto &bc : anim->channels) {
        if (bc.boneName == boneName) {
          boneChannel = &bc;
          break;
        }
      }
      if (!boneChannel) {
        anim->channels.push_back({boneName, {}});
        boneChannel = &anim->channels.back();
      }

      if (ch.target_path == cgltf_animation_path_type_translation) {
        std::vector<glm::vec3> values(keyCount);
        for (size_t i = 0; i < keyCount; ++i)
          cgltf_accessor_read_float(sampler.output, i,
                                    glm::value_ptr(values[i]), 3);
        for (size_t i = 0; i < keyCount; ++i) {
          Keyframe kf{times[i]};
          kf.position = values[i];
          kf.hasPosition = true;
          boneChannel->positionKeys.push_back(kf);
        }
      } else if (ch.target_path == cgltf_animation_path_type_rotation) {
        std::vector<glm::vec4> values(keyCount);
        for (size_t i = 0; i < keyCount; ++i) {
          cgltf_accessor_read_float(sampler.output, i,
                                    glm::value_ptr(values[i]), 4);
        }
        for (size_t i = 0; i < keyCount; ++i) {
          glm::quat q(values[i].w, values[i].x, values[i].y,
                      values[i].z); // w,x,y,z
          Keyframe kf{times[i]};
          kf.rotation = q;
          kf.hasRotation = true;
          boneChannel->rotationKeys.push_back(kf);
        }
      } else if (ch.target_path == cgltf_animation_path_type_scale) {
        std::vector<glm::vec3> values(keyCount);
        for (size_t i = 0; i < keyCount; ++i) {
          cgltf_accessor_read_float(sampler.output, i,
                                    glm::value_ptr(values[i]), 3);
        }
        for (size_t i = 0; i < keyCount; ++i) {
          Keyframe kf{times[i]};
          kf.scale = values[i];
          kf.hasScale = true;
          boneChannel->scaleKeys.push_back(kf);
        }
      }
    }
    // Sort keyframes in each channel
    for (auto &bc : anim->channels) {
      auto byTime = [](const Keyframe &a, const Keyframe &b) {
        return a.time < b.time;
      };
      std::sort(bc.positionKeys.begin(), bc.positionKeys.end(), byTime);
      std::sort(bc.rotationKeys.begin(), bc.rotationKeys.end(), byTime);
      std::sort(bc.scaleKeys.begin(), bc.scaleKeys.end(), byTime);
    }
    result.animations.push_back(anim);
  }

  const cgltf_accessor *posAcc = nullptr;
  const cgltf_accessor *normAcc = nullptr;
  const cgltf_accessor *uvAcc = nullptr;

  for (size_t i = 0; i < prim.attributes_count; ++i) {
    const auto &attr = prim.attributes[i];
    if (strcmp(attr.name, "POSITION") == 0)
      posAcc = attr.data;
    else if (strcmp(attr.name, "NORMAL") == 0)
      normAcc = attr.data;
    else if (strcmp(attr.name, "TEXCOORD_0") == 0)
      uvAcc = attr.data;
  }

  if (!posAcc) {
    LogInfo("[GltfLoader] ERROR: No POSITION attribute!");
    cgltf_free(data);
    return result;
  }

  size_t vCount = posAcc->count;
  LOG_DEBUG("[GltfLoader] Vertex count: " + std::to_string(vCount));

  // Прямое чтение позиций из буфера
  std::vector<glm::vec3> positions(vCount);
  if (posAcc->buffer_view && posAcc->buffer_view->buffer->data) {
    const uint8_t *bufferData =
        static_cast<const uint8_t *>(posAcc->buffer_view->buffer->data);
    size_t offset = posAcc->offset + posAcc->buffer_view->offset;
    size_t stride = posAcc->buffer_view->stride ? posAcc->buffer_view->stride
                                                : sizeof(float) * 3;
    for (size_t i = 0; i < vCount; ++i) {
      const float *pos =
          reinterpret_cast<const float *>(bufferData + offset + i * stride);
      positions[i] = glm::vec3(pos[0], pos[1], pos[2]);
    }
  } else {
    LogInfo("[GltfLoader] ERROR: No buffer data for positions!");
    cgltf_free(data);
    return result;
  }

  // Прямое чтение нормалей
  std::vector<glm::vec3> normals(vCount, glm::vec3(0.0f, 1.0f, 0.0f));
  if (normAcc && normAcc->buffer_view && normAcc->buffer_view->buffer->data) {
    const uint8_t *bufferData =
        static_cast<const uint8_t *>(normAcc->buffer_view->buffer->data);
    size_t offset = normAcc->offset + normAcc->buffer_view->offset;
    size_t stride = normAcc->buffer_view->stride ? normAcc->buffer_view->stride
                                                 : sizeof(float) * 3;

    for (size_t i = 0; i < vCount; ++i) {
      const float *norm =
          reinterpret_cast<const float *>(bufferData + offset + i * stride);
      normals[i] = glm::vec3(norm[0], norm[1], norm[2]);
    }
  }

  // Чтение UV координат
  std::vector<glm::vec2> uvs(vCount, glm::vec2(0.0f));
  if (uvAcc && uvAcc->buffer_view && uvAcc->buffer_view->buffer->data) {
    const uint8_t *bufferData =
        static_cast<const uint8_t *>(uvAcc->buffer_view->buffer->data);
    size_t offset = uvAcc->offset + uvAcc->buffer_view->offset;
    size_t stride = uvAcc->buffer_view->stride ? uvAcc->buffer_view->stride
                                               : sizeof(float) * 2;
    LogInfo("[GltfLoader] Reading UVs");
    for (size_t i = 0; i < vCount; ++i) {
      const float *uv =
          reinterpret_cast<const float *>(bufferData + offset + i * stride);
      uvs[i] = glm::vec2(uv[0], uv[1]);
    }
  }

  // Чтение индексов
  std::vector<uint32_t> indices;
  if (prim.indices && prim.indices->count > 0) {
    size_t iCount = prim.indices->count;
    indices.resize(iCount);

    const cgltf_accessor *idxAcc = prim.indices;
    if (idxAcc->buffer_view && idxAcc->buffer_view->buffer->data) {
      const uint8_t *bufferData =
          static_cast<const uint8_t *>(idxAcc->buffer_view->buffer->data);
      size_t offset = idxAcc->offset + idxAcc->buffer_view->offset;
      size_t stride =
          idxAcc->buffer_view->stride
              ? idxAcc->buffer_view->stride
              : (idxAcc->component_type == cgltf_component_type_r_16u ? 2 : 4);
      if (idxAcc->component_type == cgltf_component_type_r_16u) {
        for (size_t i = 0; i < iCount; ++i) {
          const uint16_t *idx = reinterpret_cast<const uint16_t *>(
              bufferData + offset + i * stride);
          indices[i] = static_cast<uint32_t>(*idx);
        }
      } else {
        for (size_t i = 0; i < iCount; ++i) {
          const uint32_t *idx = reinterpret_cast<const uint32_t *>(
              bufferData + offset + i * stride);
          indices[i] = *idx;
        }
      }

      LogInfo("[GltfLoader] Indices count: " + std::to_string(iCount));
      for (size_t i = 0; i < std::min(iCount, (size_t)12); ++i) {
        LOG_DEBUG("[GltfLoader] Index " + std::to_string(i) + ": " +
                  std::to_string(indices[i]));
      }
    }
  }

  const cgltf_accessor *jointsAcc = nullptr, *weightsAcc = nullptr;
  if (result.isSkinned) {
    for (size_t i = 0; i < prim.attributes_count; ++i) {
      const auto &attr = prim.attributes[i];
      if (strcmp(attr.name, "JOINTS_0") == 0)
        jointsAcc = attr.data;
      else if (strcmp(attr.name, "WEIGHTS_0") == 0)
        weightsAcc = attr.data;
    }
    if (!jointsAcc || !weightsAcc) {
      LogInfo("[GltfLoader] WARNING: Skin marked but no JOINTS_0/WEIGHTS_0. "
              "Falling back to static mesh.");
      result.isSkinned = false;
    }
  }

  if (result.isSkinned) {
    std::vector<SkinnedVertex> skinnedVertices(vCount);
    for (size_t i = 0; i < vCount; ++i) {
      skinnedVertices[i].position = positions[i];
      skinnedVertices[i].normal = normals[i];
      skinnedVertices[i].uv = uvs[i];
      for (int j = 0; j < 4; ++j) {
        skinnedVertices[i].boneIds[j] = -1;
        skinnedVertices[i].boneWeights[j] = 0.0f;
      }
    }

    std::vector<glm::vec4> weights(vCount);
    for (size_t i = 0; i < vCount; ++i)
      cgltf_accessor_read_float(weightsAcc, i, glm::value_ptr(weights[i]), 4);

    if (jointsAcc->buffer_view && jointsAcc->buffer_view->buffer->data) {
      const uint8_t *buf =
          static_cast<const uint8_t *>(jointsAcc->buffer_view->buffer->data);
      size_t offset = jointsAcc->offset + jointsAcc->buffer_view->offset;
      size_t stride = jointsAcc->buffer_view->stride
                          ? jointsAcc->buffer_view->stride
                          : 4; // 4 байта на 4 индекса (если uint8)
      for (size_t i = 0; i < vCount; ++i) {
        const uint8_t *ptr = buf + offset + i * stride;
        if (jointsAcc->component_type == cgltf_component_type_r_16u) {
          const uint16_t *j16 = reinterpret_cast<const uint16_t *>(ptr);
          for (int j = 0; j < 4; ++j)
            skinnedVertices[i].boneIds[j] = static_cast<int>(j16[j]);
        } else if (jointsAcc->component_type == cgltf_component_type_r_8u) {
          for (int j = 0; j < 4; ++j)
            skinnedVertices[i].boneIds[j] = static_cast<int>(ptr[j]);
        } else {
          const uint32_t *j32 = reinterpret_cast<const uint32_t *>(ptr);
          for (int j = 0; j < 4; ++j)
            skinnedVertices[i].boneIds[j] = static_cast<int>(j32[j]);
        }
        for (int j = 0; j < 4; ++j)
          skinnedVertices[i].boneWeights[j] = weights[i][j];
      }
    }

    result.mesh = std::make_shared<Mesh>(skinnedVertices, indices);
  } else {
    std::vector<StaticVertex> staticVertices(vCount);
    for (size_t i = 0; i < vCount; ++i) {
      staticVertices[i].position = positions[i];
      staticVertices[i].normal = normals[i];
      staticVertices[i].uv = uvs[i];
    }
    result.mesh = std::make_shared<Mesh>(staticVertices, indices);
    result.isSkinned = false;
  }
  cgltf_free(data);
  return result;
}

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
