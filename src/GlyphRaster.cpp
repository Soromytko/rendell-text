#include <GlyphRaster.h>

namespace rendell_text {
void GlyphRaster::rasterize(const ShapeResult &shapeResult,
                            std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache,
                            AtlasType atlasType) {
    for (const rendell_text::ShapedGlyph shapedGlyph : shapeResult.shapedGlyphs) {
        if (!glyphAtlasCache->contains(shapedGlyph.id)) {
            
            //glyphAtlasCache.append()
        }
    }
}
} // namespace rendell_text