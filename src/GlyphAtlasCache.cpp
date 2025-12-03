#include <GlyphAtlasCache.h>

#include <SkylineGlyphAtlas.h>

#include <algorithm>

namespace rendell_text {
GlyphAtlasCache::GlyphAtlasCache(IFontRasterSharedPtr fontRaster, AtlasType atlasType)
    : _fontRaster(fontRaster)
    , _atlasType(atlasType) {
    assert(_fontRaster);
    // TODO: Needs to be implemented.
    assert(_atlasType != AtlasType::mtsdf);
    _fontRaster = fontRaster;
    _atlasType = atlasType;
    addAtlas();
    assert(_atlases.size() > 0);
}

uint32_t GlyphAtlasCache::getGlyphWidth() const {
    assert(_fontRaster);
    return _fontRaster->getGlyphWidth();
}

uint32_t GlyphAtlasCache::getGlyphHeight() const {
    assert(_fontRaster);
    return _fontRaster->getGlyphWidth();
}

uint32_t GlyphAtlasCache::getAtlasCount() const {
    return static_cast<uint32_t>(_atlases.size());
}

uint32_t GlyphAtlasCache::getFontHeight() const {
    assert(_fontRaster);
    return _fontRaster->getFontHeight();
}

std::vector<size_t> GlyphAtlasCache::getAtlasVersions() const {
    std::vector<size_t> result;
    result.reserve(_atlases.size());
    std::transform(_atlases.begin(), _atlases.end(), std::back_inserter(result),
                   [](const auto &atlas) { return atlas->getVersion(); });
    return result;
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
    assert(glyph.bitmap.glyphSize.x <= atlas->getWidth());
    assert(glyph.bitmap.glyphSize.y <= atlas->getHeight());

    IGlyphAtlas::GlyphInfo glyphInfo;
    if (!atlas->tryInsert(glyph.bitmap, glyphInfo)) {
        atlas = addAtlas();
        assert(atlas);
        if (!atlas->tryInsert(glyph.bitmap, glyphInfo)) {
            assert(false);
        }
        return glyph;
    }
}

IGlyphAtlas *GlyphAtlasCache::addAtlas() {
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