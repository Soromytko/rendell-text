#pragma once
#include <rendell_text/types.h>

#include <vector>

namespace rendell_text {
struct GlyphBitmap {
    Size size;
    std::vector<std::byte> pixels;
};

struct RasterizedGlyph final {
    GlyphId id;
    float bearingX;
    float bearingY;
    float advance;
    AtlasType atlasType;
    GlyphBitmap bitmap;
};

using RasterizedGlyphList = std::vector<RasterizedGlyph>;

struct RasterizeResult final {
    FontInstance fontInstance;
    RasterizedGlyphList rasterisedGlyphs;
};
} // namespace rendell_text