#pragma once
#include <glm/glm.hpp>
#include <map>
#include <rendell/oop/rendell_oop.h>
#include <rendell/rendell.h>

namespace rendell_text {
struct RasterizedChar {
    wchar_t character{};
    glm::ivec2 glyphSize{};
    glm::ivec2 glyphBearing{};
    uint32_t glyphAdvance{};
};

struct FontRasterizationResult {
    rendell::oop::Texture2DArraySharedPtr texture2DArray{};
    std::vector<RasterizedChar> rasterizedChars{};
};

} // namespace rendell_text
