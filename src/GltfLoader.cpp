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
  //49

  //--------------------------------------
  LogInfo("[GltfLoader] load success");
  cgltf_free(data);
  return result;
}
