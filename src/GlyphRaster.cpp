#include <GlyphRaster.h>

#include "FontRaster.h"

#include <cassert>

namespace rendell_text {
GlyphRaster::GlyphRaster()
    : _raster(std::make_unique<FontRaster>()) {
    assert(_raster);
}

RasterizedResult GlyphRaster::rasterize(const ShapeResult &shapeResult, AtlasType atlasType) {
    RasterizedResult result{
        .fontInstance = shapeResult.fontInstance,
        .rasterisedGlyphs = {},
    };
    result.rasterisedGlyphs.reserve(shapeResult.shapedGlyphs.size());
    for (const rendell_text::ShapedGlyph &shapedGlyph : shapeResult.shapedGlyphs) {
        //_raster->rasterizeGlyph()
        // result.rasterisedGlyphs =
    }

    return result;
}
} // namespace rendell_text