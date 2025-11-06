#pragma once
#include <rendell_text/types.h>

#include <vector>

namespace rendell_text {
struct ShapedGlyph {
    GlyphId id;
    float advanceX;
    float advanceY;
    float offsetX;
    float offsetY;
    uint32_t cluster;
};

using ShapedGlyphList = std::vector<ShapedGlyph>;

struct ShapeResult {
    FontInstance fontInstance;
    ShapedGlyphList shapedGlyphs;
};
} // namespace rendell_text