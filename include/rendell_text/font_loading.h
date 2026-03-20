#pragma once
#include <rendell_text/IFont.h>

#include <filesystem>
#include <memory>

namespace rendell_text {
std::shared_ptr<IFont> loadFont(const std::filesystem::path &path);
} // namespace rendell_text