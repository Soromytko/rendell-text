#include <rendell_text/factory.h>

#include "FontRaster.h"
#include <GlyphAtlasCache.h>
#include <TextLayout.h>
#include <TextRenderer.h>

namespace rendell_text {
std::shared_ptr<IFontRaster> createFontRaster() {
    std::shared_ptr<FontRaster> result = std::make_shared<FontRaster>();
    return result;
}

std::shared_ptr<IGlyphAtlasCache> createGlyphAtlasCache(std::shared_ptr<IFontRaster> fontRaster,
                                                        AtlasType atlasType = AtlasType::msdf) {
    assert(fontRaster);
    return std::make_shared<GlyphAtlasCache>(fontRaster, atlasType);
}

std::shared_ptr<ITextLayout> createTextLayout(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    return std::make_shared<TextLayout>(glyphAtlasCache);
}

std::shared_ptr<ITextRenderer> createTextRenderer(std::shared_ptr<ITextLayout> textLayout) {
    assert(textLayout);
    return std::make_shared<TextRenderer>(textLayout);
}
} // namespace rendell_text