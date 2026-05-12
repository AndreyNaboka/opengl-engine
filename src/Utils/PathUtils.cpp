#include "Utils/PathUtils.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

namespace Path {
std::filesystem::path GetExecutableDirectory() {
#ifdef _WIN32
  char buffer[MAX_PATH];
  if (GetModuleFileNameA(nullptr, buffer, MAX_PATH)) {
    return std::filesystem::path(buffer).parent_path();
  }
#elif __linux__
  char buffer[4096];
  ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (len != -1) {
    buffer[len] = '\0';
    return std::filesystem::path(buffer).parent_path();
  }
#elif __APPLE__
  char buffer[4096];
  uint32_t size = sizeof(buffer);
  if (_NSGetExecutablePath(buffer, &size) == 0) {
    return std::filesystem::path(buffer).parent_path();
  }
#endif
  // Fallback: если не удалось определить путь к exe, используем текущий каталог
  return std::filesystem::current_path();
}

std::filesystem::path ResolveAssetPath(const std::string &relativePath) {
  return GetExecutableDirectory() / relativePath;
}
} // namespace Path
