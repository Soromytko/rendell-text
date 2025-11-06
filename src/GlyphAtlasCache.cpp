#include <rendell_text/GlyphAtlasCache.h>

namespace rendell_text {
GlyphAtlasCache::GlyphAtlasCache(FontSharedPtr font, AtlasType atlasType) {
    assert(font);
    // TODO: Needs to be implemented.
    assert(atlasType != AtlasType::mtsdf);
    _font = font;
    _atlasType = atlasType;
}

FontSharedPtr GlyphAtlasCache::getFont() const {
    return _font;
}

const GlyphBitmap &GlyphAtlasCache::getGlyphBitmap() const {
    // TODO: insert return statement here
}
} // namespace rendell_text