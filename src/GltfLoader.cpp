#include "GltfLoader.h"
#include "Utils/PathUtils.h"
#include "Utils/Logger.h"
#include <cgltf.h>
#include <cstdint>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>
#include <memory>

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

static std::shared_ptr<Texture>
LoadTextureFromCgltf(const cgltf_image *image, const std::string &basePath) {
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

GltfModelData GltfLoader::Load(const std::string &assetPath) {
  GltfModelData result;
  auto absPath = Path::ResolveAssetPath(assetPath);

  LOG_DEBUG("[GltfLoader] ========== START LOADING ==========");
  LOG_DEBUG("[GltfLoader] File: " + absPath.string());

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

  const cgltf_mesh *targetMesh = &data->meshes[0];
  const cgltf_primitive &prim = targetMesh->primitives[0];

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

    LOG_DEBUG("[GltfLoader] Reading positions: offset=" +
              std::to_string(offset) + ", stride=" + std::to_string(stride));

    for (size_t i = 0; i < vCount; ++i) {
      const float *pos =
          reinterpret_cast<const float *>(bufferData + offset + i * stride);
      positions[i] = glm::vec3(pos[0], pos[1], pos[2]);
      LOG_DEBUG("[GltfLoader] Vertex " + std::to_string(i) + " position: (" +
                std::to_string(positions[i].x) + ", " +
                std::to_string(positions[i].y) + ", " +
                std::to_string(positions[i].z) + ")");
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

    LOG_DEBUG("[GltfLoader] Reading normals: offset=" + std::to_string(offset) +
              ", stride=" + std::to_string(stride));

    for (size_t i = 0; i < vCount; ++i) {
      const float *norm =
          reinterpret_cast<const float *>(bufferData + offset + i * stride);
      normals[i] = glm::vec3(norm[0], norm[1], norm[2]);
      if (i < 3) {
        LOG_DEBUG("[GltfLoader] Normal " + std::to_string(i) + ": (" +
                  std::to_string(normals[i].x) + ", " +
                  std::to_string(normals[i].y) + ", " +
                  std::to_string(normals[i].z) + ")");
      }
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

      LOG_DEBUG("[GltfLoader] Reading indices: offset=" +
                std::to_string(offset) + ", stride=" + std::to_string(stride));

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

  // Создаем вершины
  std::vector<StaticVertex> staticVertices(vCount);
  for (size_t i = 0; i < vCount; ++i) {
    staticVertices[i].position = positions[i];
    staticVertices[i].normal = normals[i];
    staticVertices[i].uv = glm::vec2(0.0f); // UV пока не используем
  }

  // Создаем меш
  result.mesh = std::make_shared<Mesh>(staticVertices, indices);
  result.isSkinned = false;

  cgltf_free(data);
  LOG_DEBUG("[GltfLoader] ========== LOADING COMPLETE ==========");

  return result;
}
