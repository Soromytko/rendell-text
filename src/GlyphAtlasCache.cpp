#include <rendell_text/GlyphAtlasCache.h>

namespace rendell_text {
GlyphAtlasCache::GlyphAtlasCache(IFontRasterSharedPtr fontRaster, AtlasType atlasType) {
    assert(fontRaster);
    // TODO: Needs to be implemented.
    assert(atlasType != AtlasType::mtsdf);
    _fontRaster = fontRaster;
    _atlasType = atlasType;
}

const GlyphBitmap &GlyphAtlasCache::getGlyphBitmap(Codepoint character) const {
    assert(_fontRaster);

    auto it = _glyphs.find(character);
    if (it != _glyphs.end()) {
        return it->second;
    }

    GlyphBitmap &glyphBitmap = _glyphs[character];
    if (!_fontRaster->rasterizeGlyph(character, _atlasType, glyphBitmap)) {
        assert(false);
    }
    return glyphBitmap;
}
} // namespace rendell_text