#pragma once
#include <IFontRaster.h>

#include <filesystem>
#include <memory>

namespace rendell_text {
IFontRasterSharedPtr loadFont(const std::filesystem::path &path);
} // namespace rendell_text
