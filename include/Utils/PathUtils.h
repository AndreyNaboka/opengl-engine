#pragma once
#include <filesystem>
#include <string>

namespace Path {
std::filesystem::path GetExecutableDirectory();
std::filesystem::path ResolveAssetPath(const std::string &relativePath);
} // namespace Path
