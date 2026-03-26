#pragma once
#include <rendell_text/IGlyphRaster.h>

namespace rendell_text {
class GlyphRaster final : public IGlyphRaster {
public:
    void rasterize(const ShapeResult &shapeResult,
                   std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache, AtlasType atlasType) override;
};
} // namespace rendell_text