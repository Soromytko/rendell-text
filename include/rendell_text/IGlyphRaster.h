#pragma once
#include <rendell_text/GlyphBitmap.h>
#include <rendell_text/IGlyphAtlasCache.h>
#include <rendell_text/ShapeResult.h>

namespace rendell_text {
class IGlyphRaster {
public:
    IGlyphRaster() = default;
    virtual ~IGlyphRaster() = default;

    virtual void rasterize(const ShapeResult &shapeResult,
                           std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache,
                           AtlasType atlasType) = 0;
};
} // namespace rendell_text