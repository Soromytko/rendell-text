#pragma once
#include "types.h"

#include <glm/glm.hpp>
#include <vector>

namespace rendell_text {
enum class AtlasType {
    bitmap,
    sdf,
    msdf,
    mtsdf,
};

struct GlyphBitmap {
    Codepoint character{};
    uint32_t width{};
    uint32_t height{};
    glm::ivec2 glyphBearing{};
    uint32_t glyphAdvance{};
    AtlasType atlasType{};
    std::vector<std::byte> pixels{};
};

struct AtlasConfig {
    AtlasType type = AtlasType::msdf;
    uint32_t width;
    uint32_t height;
};

} // namespace rendell_text
