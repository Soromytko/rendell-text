#include <rendell_text/GlyphAtlasCache.h>

#include <SkylineGlyphAtlas.h>

namespace rendell_text {
GlyphAtlasCache::GlyphAtlasCache(IFontRasterSharedPtr fontRaster, AtlasType atlasType) {
    assert(fontRaster);
    // TODO: Needs to be implemented.
    assert(atlasType != AtlasType::mtsdf);
    _fontRaster = fontRaster;
    _atlasType = atlasType;
}

const std::vector<std::unique_ptr<IGlyphAtlas>> &GlyphAtlasCache::getAtlases() const {
    return _atlases;
}

const Glyph &GlyphAtlasCache::getOrRasterizeGlyph(Codepoint character) {
    assert(_fontRaster);

    auto it = _glyphs.find(character);
    if (it != _glyphs.end()) {
        return it->second;
    }

    Glyph &glyph = _glyphs[character];
    if (!_fontRaster->rasterizeGlyph(character, _atlasType, glyph.bitmap)) {
        assert(false);
    }

    IGlyphAtlas *atlas = getCurrentAtlas();
    assert(atlas);

    IGlyphAtlas::GlyphInfo glyphInfo;
    if (!atlas->tryInsert(glyph.bitmap, glyphInfo)) {
        atlas = createAtlas();
        assert(atlas);
        if (!atlas->tryInsert(glyph.bitmap, glyphInfo)) {
            assert(false);
        }

        return glyph;
    }
}

IGlyphAtlas *GlyphAtlasCache::createAtlas() {
    std::unique_ptr<SkylineGlyphAtlas> atlas = std::make_unique<SkylineGlyphAtlas>();
    IGlyphAtlas *result = atlas.get();
    _atlases.push_back(std::move(atlas));
    return result;
}

IGlyphAtlas *GlyphAtlasCache::getCurrentAtlas() const {
    assert(_atlases.size() > 0);
    return _atlases[_atlases.size() - 1].get();
}
} // namespace rendell_text