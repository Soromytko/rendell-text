#include <rendell_text/FontLoading.h>

#include "FontRaster.h"
#include <logging.h>

#include <cassert>

namespace rendell_text {
IFontRasterSharedPtr loadFont(const std::filesystem::path &path) {
    assert(!path.empty());

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        RT_ERROR("Failed to open font file: {}", path.string());
        return nullptr;
    }

    std::streamsize size = file.tellg();
    if (size <= 0) {
        RT_ERROR("Font file is empty: {}", path.string());
        return nullptr;
    }
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char *>(buffer.data()), size)) {
        RT_ERROR("Failed to read font file: {}", path.string());
        return nullptr;
    }

    FontRasterSharedPtr fontRaster = makeFontRaster();
    fontRaster->setFontData(buffer.data(), buffer.size());
    return fontRaster;
}
} // namespace rendell_text