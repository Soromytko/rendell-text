#pragma once
#include <rendell_text/RasterizedGlyph.h>

#include <span>

namespace rendell_text {
class IGlyphRaster {
public:
    IGlyphRaster() = default;
    virtual ~IGlyphRaster() = default;

    virtual RasterizeResult rasterize(std::span<const GlyphId> glyphs, FontInstance fontInstance,
                                      AtlasType atlasType) = 0;
    virtual RasterizedGlyph rasterize(GlyphId glyphId, FontInstance fontInstance,
                                      AtlasType atlasType) = 0;
};
} // namespace rendell_text