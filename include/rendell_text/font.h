#pragma once
#include <rendell_text/types.h>

#include <filesystem>

namespace rendell_text {
FontHandle loadFont(const std::filesystem::path &path);
bool isFontValid(FontHandle fontHandle);
void releaseFont(FontHandle fontHandle);
} // namespace rendell_text