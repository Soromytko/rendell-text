#pragma once
#include <rendell_text/IGlyphRaster.h>

#include <memory>

namespace rendell_text {
class IFontRaster;

class GlyphRaster final : public IGlyphRaster {
public:
    GlyphRaster();

    RasterizedResult rasterize(const ShapeResult &shapeResult, AtlasType atlasType) override;

private:
    std::unique_ptr<IFontRaster> _raster;
};
} // namespace rendell_text