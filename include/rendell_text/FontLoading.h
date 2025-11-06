#pragma once
#include "Font.h"

#include <filesystem>
#include <memory>

namespace rendell_text {
std::shared_ptr<Font> loadFont(const std::filesystem::path &path);
}