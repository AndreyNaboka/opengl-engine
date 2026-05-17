#include <cgltf.h>
#include <glm/gtc/quaternion.hpp>
#include "GltfLoader.h"
#include "Mesh.h"
#include "Utils/PathUtils.h"
#include "Utils/Logger.h"
#include <algorithm>
#include <string>
#include <cstring>

GltfModelData GltfLoader::Load(const std::string &assetPath) {
  GltfModelData result;
  // open and parse file
  auto absPath = Path::ResolveAssetPath(assetPath);
  cgltf_options opts = {};
  cgltf_data *data = nullptr; // Указатель на распарсенную сцену glTF
  LogInfo("[GltfLoader] try to load and parse model: " + assetPath);
  cgltf_result res = cgltf_parse_file(&opts, absPath.string().c_str(), &data);
  if (res != cgltf_result_success)
    LogInfo("[GltfLoader] cgltf_parse failed: " + std::to_string(res));
  //--------------------------------------

  // read and fill GltfModelData
  res = cgltf_load_buffers(&opts, data, absPath.string().c_str());
  if (res != cgltf_result_success)
    LogInfo("[GltfLoader] load buffers failed");

  if (data->meshes_count == 0) {
    cgltf_free(data);
    LogInfo("[GltfLoader] empty model");
    return result;
  }

  const cgltf_mesh &mesh = data->meshes[0];
  const cgltf_primitive &prim = mesh.primitives[0];
  bool isSkinned = false;
  cgltf_skin *activeSkin = nullptr;
  for (size_t n = 0; n < data->nodes_count; ++n) {
    const cgltf_node *node = &data->nodes[n];
    if (node->mesh == &data->meshes[0] && node->skin != nullptr) {
      isSkinned = true;
      activeSkin = node->skin;
      break;
    }
  }
  result.isSkinned = isSkinned;

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
    cgltf_free(data);
    LogInfo("[GltfLoader] Missing POSITION attrbute");
  }

  // vertex count
  const size_t vCount = posAcc->count;
  std::vector<Vertex> vertices(vCount);

  auto ReadFloatAttr = [&](const cgltf_accessor *acc, float *out,
                           size_t count) {
    if (acc)
      cgltf_accessor_read_float(acc, 0, out, count);
  };
  ReadFloatAttr(posAcc, reinterpret_cast<float *>(&vertices[0].position),
                vCount * 3);
  ReadFloatAttr(normAcc, reinterpret_cast<float *>(&vertices[0].normal),
                vCount * 3);
  if (uvAcc) {
    std::vector<float> temp(vCount * 2);
    cgltf_accessor_read_float(uvAcc, 0, temp.data(), vCount * 2);
    for (size_t i = 0; i < vCount; ++i)
      vertices[i].uv = {temp[i * 2], temp[i * 2 + 1]};
  }

  if (!isSkinned && activeSkin) {
    result.inverseBindMatrices.resize(activeSkin->joints_count);
    // read inverse bind matrices
    if (activeSkin->inverse_bind_matrices) {
      cgltf_accessor_read_float(
          activeSkin->inverse_bind_matrices, 0,
          reinterpret_cast<float *>(result.inverseBindMatrices.data()),
          activeSkin->joints_count * 16);
    } else {
      for (auto &m : result.inverseBindMatrices)
        m = glm::mat4(1.0f);
    }
    for (size_t i = 0; i < activeSkin->joints_count; ++i) {
      const cgltf_node *jointNode = activeSkin->joints[i];
      const std::string name =
          jointNode->name ? jointNode->name : ("joint_" + std::to_string(i));
      result.boneNameToIndex[name] = i;
    }
  }

  //--------------------------------------
  LogInfo("[GltfLoader] load success");
  cgltf_free(data);
  return result;
}
