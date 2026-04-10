#pragma once
#include <rendell_text/FontHandle.h>

#include <filesystem>
#include <memory>

namespace rendell_text {
// std::shared_ptr<IFont> loadFont(const std::filesystem::path &path);
FontHandle loadFont(const std::filesystem::path &path);
} // namespace rendell_text