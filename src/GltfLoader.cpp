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
#include <string>

static const char *GltfResultToString(cgltf_result res);

static bool ReadAccessorVec2(const cgltf_accessor *accessor, size_t index,
                             glm::vec2 &value) {
  float components[2] = {0.0f, 0.0f};
  if (!accessor ||
      !cgltf_accessor_read_float(accessor, index, components, 2)) {
    return false;
  }
  value = glm::vec2(components[0], components[1]);
  return true;
}

static bool ReadAccessorVec3(const cgltf_accessor *accessor, size_t index,
                             glm::vec3 &value) {
  float components[3] = {0.0f, 0.0f, 0.0f};
  if (!accessor ||
      !cgltf_accessor_read_float(accessor, index, components, 3)) {
    return false;
  }
  value = glm::vec3(components[0], components[1], components[2]);
  return true;
}

static bool ReadAccessorVec4(const cgltf_accessor *accessor, size_t index,
                             glm::vec4 &value) {
  float components[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  if (!accessor ||
      !cgltf_accessor_read_float(accessor, index, components, 4)) {
    return false;
  }
  value = glm::vec4(components[0], components[1], components[2], components[3]);
  return true;
}

static bool ReadAccessorJoints(const cgltf_accessor *accessor, size_t index,
                               int (&joints)[4]) {
  cgltf_uint components[4] = {0, 0, 0, 0};
  if (!accessor || !cgltf_accessor_read_uint(accessor, index, components, 4)) {
    return false;
  }
  for (size_t i = 0; i < 4; ++i) {
    joints[i] = static_cast<int>(components[i]);
  }
  return true;
}

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
      LogInfo("[GltfLoader] Loading embedded texture, size: " +
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
      texture = std::make_shared<Texture>(texturePath);
      LogInfo("[GltfLoader] Loaded external texture: " + texturePath);
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
  LogInfo("[GltfLoader] Found " + std::to_string(data->materials_count) +
          " materials");
  result.materials.reserve(data->materials_count);
  for (size_t i = 0; i < data->materials_count; ++i) {
    GltfModelData::Material material =
        LoadMaterial(&data->materials[i], basePath);
    result.materials.push_back(material);
  }

  const cgltf_mesh *targetMesh = &data->meshes[0];
  if (targetMesh->primitives_count == 0) {
    LogInfo("[GltfLoader] Mesh has no primitives");
    cgltf_free(data);
    return result;
  }
  const cgltf_primitive &firstPrim = targetMesh->primitives[0];

  if (firstPrim.material) {
    for (size_t i = 0; i < data->materials_count; ++i) {
      if (&data->materials[i] == firstPrim.material) {
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
  LogInfo("[GltfLoader] Start load " + std::to_string(data->animations_count) +
          " animations");
  for (size_t a = 0; a < data->animations_count; ++a) {
    auto anim = std::make_shared<Animation>();
    const cgltf_animation &cgAnim = data->animations[a];
    anim->duration = 0.0f;
    anim->name = cgAnim.name ? std::string(cgAnim.name) : "";

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
    LogInfo("[GltfLoader]   [+] \"" + anim->name + "\"");
  }

  std::vector<StaticVertex> staticVertices;
  std::vector<SkinnedVertex> skinnedVertices;
  std::vector<uint32_t> indices;
  bool canUseSkinning = result.isSkinned;

  for (size_t primitiveIndex = 0; primitiveIndex < targetMesh->primitives_count;
       ++primitiveIndex) {
    const cgltf_primitive &prim = targetMesh->primitives[primitiveIndex];
    if (prim.type != cgltf_primitive_type_triangles) {
      LogInfo("[GltfLoader] Skipping non-triangle primitive: " +
              std::to_string(primitiveIndex));
      continue;
    }

    const cgltf_accessor *posAcc = nullptr;
    const cgltf_accessor *normAcc = nullptr;
    const cgltf_accessor *uvAcc = nullptr;
    const cgltf_accessor *jointsAcc = nullptr;
    const cgltf_accessor *weightsAcc = nullptr;

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
      LogInfo("[GltfLoader] Skipping primitive without POSITION: " +
              std::to_string(primitiveIndex));
      continue;
    }

    const size_t vCount = posAcc->count;
    const uint32_t vertexOffset =
        static_cast<uint32_t>(canUseSkinning ? skinnedVertices.size()
                                             : staticVertices.size());
    LOG_DEBUG("[GltfLoader] Primitive " + std::to_string(primitiveIndex) +
              " vertex count: " + std::to_string(vCount));

    if (canUseSkinning && (!jointsAcc || !weightsAcc)) {
      LogInfo("[GltfLoader] WARNING: Skin marked but primitive has no "
              "JOINTS_0/WEIGHTS_0. Falling back to static mesh.");
      canUseSkinning = false;
      staticVertices.reserve(skinnedVertices.size() + vCount);
      for (const SkinnedVertex &vertex : skinnedVertices) {
        staticVertices.push_back({vertex.position, vertex.normal, vertex.uv});
      }
      skinnedVertices.clear();
    }

    if (canUseSkinning) {
      skinnedVertices.reserve(skinnedVertices.size() + vCount);
    } else {
      staticVertices.reserve(staticVertices.size() + vCount);
    }

    for (size_t i = 0; i < vCount; ++i) {
      glm::vec3 position(0.0f);
      if (!ReadAccessorVec3(posAcc, i, position)) {
        LogInfo("[GltfLoader] ERROR: Failed to read POSITION attribute!");
        cgltf_free(data);
        return result;
      }

      glm::vec3 normal(0.0f, 1.0f, 0.0f);
      if (normAcc)
        ReadAccessorVec3(normAcc, i, normal);

      glm::vec2 uv(0.0f);
      if (uvAcc)
        ReadAccessorVec2(uvAcc, i, uv);

      if (canUseSkinning) {
        SkinnedVertex vertex{};
        vertex.position = position;
        vertex.normal = normal;
        vertex.uv = uv;
        for (int j = 0; j < 4; ++j) {
          vertex.boneIds[j] = -1;
          vertex.boneWeights[j] = 0.0f;
        }

        glm::vec4 weights(0.0f);
        ReadAccessorVec4(weightsAcc, i, weights);
        ReadAccessorJoints(jointsAcc, i, vertex.boneIds);
        for (int j = 0; j < 4; ++j) {
          vertex.boneWeights[j] = weights[j];
        }
        skinnedVertices.push_back(vertex);
      } else {
        staticVertices.push_back({position, normal, uv});
      }
    }

    if (prim.indices && prim.indices->count > 0) {
      const size_t iCount = prim.indices->count;
      const cgltf_accessor *idxAcc = prim.indices;
      indices.reserve(indices.size() + iCount);
      for (size_t i = 0; i < iCount; ++i) {
        indices.push_back(vertexOffset +
                          static_cast<uint32_t>(
                              cgltf_accessor_read_index(idxAcc, i)));
      }
      LogInfo("[GltfLoader] Primitive " + std::to_string(primitiveIndex) +
              " indices count: " + std::to_string(iCount));
    } else {
      indices.reserve(indices.size() + vCount);
      for (uint32_t i = 0; i < vCount; ++i) {
        indices.push_back(vertexOffset + i);
      }
      LogInfo("[GltfLoader] Primitive " + std::to_string(primitiveIndex) +
              " is non-indexed");
    }
  }

  if (indices.empty()) {
    LogInfo("[GltfLoader] ERROR: No drawable primitives found");
    cgltf_free(data);
    return result;
  }

  if (canUseSkinning) {
    result.mesh = std::make_shared<Mesh>(skinnedVertices, indices);
  } else {
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
