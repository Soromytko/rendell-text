#pragma once
#include <rendell_text/types.h>

#include <vector>

namespace rendell_text {
enum class AtlasType {
    bitmap,
    sdf,
    msdf,
    mtsdf,
};

struct GlyphBitmap {
    Size size;
    std::vector<std::byte> pixels;
};

struct RasterizedGlyph final {
    GlyphId id;
    int glyphBearingX;
    int glyphBearingY;
    float glyphAdvance;
    AtlasType atlasType;
    GlyphBitmap bitmap;
};

using RasterizedGlyphList = std::vector<RasterizedGlyph>;

struct RasterizeResult final {
    FontInstance fontInstance;
    RasterizedGlyphList rasterisedGlyphs;
};
} // namespace rendell_text