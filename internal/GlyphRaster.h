#pragma once
#include <rendell_text/IGlyphRaster.h>

namespace rendell_text {
struct MSDF_Resource;

class GlyphRaster final : public IGlyphRaster {
public:
    GlyphRaster() = default;
    ~GlyphRaster() = default;

    RasterizeResult rasterize(std::span<const GlyphId> glyphs, FontInstance fontInstance,
                              AtlasType atlasType) override;
    RasterizedGlyph rasterize(GlyphId glyphId, FontInstance fontInstance,
                              AtlasType atlasType) override;

private:
    bool rasterizeMSDF(std::span<const GlyphId> glyphs, Size fontSize, const MSDF_Resource &msdf,
                       RasterizedGlyphList &result) const;
};
} // namespace rendell_text