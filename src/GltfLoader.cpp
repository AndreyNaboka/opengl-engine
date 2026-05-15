#include <cgltf.h>
#include <glm/gtc/quaternion.hpp>
#include "GltfLoader.h"
#include "Utils/PathUtils.h"
#include "Utils/Logger.h"
#include <algorithm>
#include <string>

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
  const cgltf_accessor *jointAcc = nullptr;
  const cgltf_accessor *weightAcc = nullptr;
  for (size_t i = 0; i < prim.attributes_count; ++i) {
  }

  //--------------------------------------
  LogInfo("[GltfLoader] load success");
  cgltf_free(data);
  return result;
}
