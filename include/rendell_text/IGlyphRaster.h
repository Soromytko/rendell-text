#pragma once
#include <rendell_text/GlyphBitmap.h>
#include <rendell_text/RasterizeResult.h>
#include <rendell_text/ShapeResult.h>

namespace rendell_text {
class IGlyphRaster {
public:
    IGlyphRaster() = default;
    virtual ~IGlyphRaster() = default;

    virtual RasterizedResult rasterize(const ShapeResult &shapeResult, AtlasType atlasType) = 0;
};
} // namespace rendell_text