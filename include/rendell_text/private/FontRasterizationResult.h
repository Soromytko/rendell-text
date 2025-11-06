#pragma once
#include <glm/glm.hpp>
#include <rendell/oop/rendell_oop.h>
#include <rendell/rendell.h>

#include <map>
#include <memory>
#include <vector>

namespace rendell_text {
struct RasterizedChar {
    wchar_t character{};
    glm::ivec2 glyphSize{};
    glm::ivec2 glyphBearing{};
    uint32_t glyphAdvance{};
    std::vector<std::byte> pixels{};
};

using RasterizedGlyphList = std::vector<RasterizedChar>;
using RasterizedGlyphListSharedPtr = std::shared_ptr<RasterizedGlyphList>;
using RasterizedGlyphListWeakPtr = std::weak_ptr<RasterizedGlyphList>;

inline RasterizedGlyphListSharedPtr createEmptyRasterizedGlyphList() {
    return std::make_shared<RasterizedGlyphList>();
}

struct FontRasterizationResult {
    rendell::oop::Texture2DArraySharedPtr texture2DArray{};
    std::vector<RasterizedChar> rasterizedChars{};
};

} // namespace rendell_text
