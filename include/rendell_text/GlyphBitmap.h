#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace rendell_text {
enum class AtlasType {
    bitmap,
    sdf,
    msdf,
    mtsdf,
};

using Codepoint = char32_t;

struct GlyphBitmap {
    Codepoint character{};
    glm::ivec2 glyphSize{};
    glm::ivec2 glyphBearing{};
    uint32_t glyphAdvance{};
    AtlasType atlasType{};
    std::vector<std::byte> pixels{};
};

} // namespace rendell_text
