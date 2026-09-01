#pragma once
#include <rendell_text/types.h>

#include <vector>

namespace rendell_text {
struct GlyphBitmap {
    Size size;
    std::vector<std::byte> pixels;

    constexpr bool isEmpty() const { return size.area() == 0 || pixels.size() == 0; }
};

struct RasterizedGlyph final {
    GlyphId id;
    float bearingX;
    float bearingY;
    float advance;
    AtlasType atlasType;
    GlyphBitmap bitmap;

    constexpr bool isEmpty() const { return bitmap.isEmpty(); }
};

using RasterizedGlyphList = std::vector<RasterizedGlyph>;

struct RasterizeResult final {
    FontInstance fontInstance;
    RasterizedGlyphList rasterisedGlyphs;
};
} // namespace rendell_text