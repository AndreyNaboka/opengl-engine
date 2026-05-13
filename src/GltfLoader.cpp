#include <cgltf.h>
#include <glm/gtc/quaternion.hpp>
#include <fstream>
#include "GltfLoader.h"
#include "Utils/PathUtils.h"
#include "Utils/Logger.h"
#include <algorithm>
#include <string>

// Чтение бинарного/JSON файла в память. cgltf вызывает эту функцию при
// парсинге.
void *GltfLoader::CgltfReadFile(void *user, const char *path, size_t *outSize) {
  std::ifstream file(
      path,
      std::ios::binary |
          std::ios::ate); // Открываем в бинарном режиме, сразу в конец файла
  if (!file.is_open())
    return nullptr;              // Если файл не открыт → ошибка
  size_t size = file.tellg();    // Узнаём размер файла
  file.seekg(0, std::ios::beg);  // Возвращаем указатель в начало
  auto *buf = new uint8_t[size]; // Выделяем буфер нужного размера
  file.read(reinterpret_cast<char *>(buf),
            size); // Читаем всё содержимое в буфер
  *outSize = size; // Возвращаем размер через выходной параметр
  return buf;      // Возвращаем указатель на буфер (владелец = cgltf)
}

// Освобождение памяти, выделенной в CgltfReadFile
void GltfLoader::CgltfReleaseFile(void *, void *data) {
  delete[] static_cast<uint8_t *>(data); // Удаляем массив байтов
}

GltfModelData GltfLoader::Load(const std::string &assetPath) {
  GltfModelData result;
  auto absPath = Path::ResolveAssetPath(assetPath);

  cgltf_options opts = {};

  cgltf_data *data = nullptr; // Указатель на распарсенную сцену glTF
  LogInfo("[GltfLoader] try to load and parse model: " + assetPath);
  cgltf_result res = cgltf_parse_file(&opts, absPath.string().c_str(), &data);
  if (res != cgltf_result_success)
    LogInfo("[GltfLoader] cgltf_parse failed: " + std::to_string(res));

  LogInfo("[GltfLoader] free resource: " + assetPath);
  cgltf_free(data);
  return result;
}
