#pragma once
#include <rendell_text/FontInstance.h>
#include <rendell_text/types.h>

namespace rendell_text {
struct ShapedGlyph {
    GlyphId id;
    float advanceX;
    float advanceY;
    float offsetX;
    float offsetY;
    uint32_t cluster;
};

struct ShapeResult {
    FontInstance fontInstance;
    std::vector<ShapedGlyph> shapedGlyphs;
};
} // namespace rendell_text