#include <GlyphAtlasCache.h>

#include <SkylineGlyphAtlas.h>
#include <rendell_text/IFontRaster.h>

#include <algorithm>

namespace rendell_text {
GlyphAtlasCache::GlyphAtlasCache(std::shared_ptr<IFontRaster> fontRaster, AtlasConfig atlasConfig)
    : _fontRaster(fontRaster)
    , _atlasConfig(_atlasConfig) {
    assert(_fontRaster);
    // TODO: Needs to be implemented.
    assert(_atlasConfig.type != AtlasType::mtsdf);
    assert(_atlasConfig.height > 0 && _atlasConfig.width > 0);
    addAtlas();
    assert(_atlases.size() > 0);
}

size_t GlyphAtlasCache::getVersion() const {
    return _version;
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

uint32_t GlyphAtlasCache::getAscender() const {
    assert(_fontRaster);
    return _fontRaster->getAscender();
}

uint32_t GlyphAtlasCache::getDescender() const {
    assert(_fontRaster);
    return _fontRaster->getDescender();
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
    if (!_fontRaster->rasterizeGlyph(character, _atlasConfig.type, glyph.bitmap)) {
        assert(false);
    }

    IGlyphAtlas *atlas = getCurrentAtlas();
    assert(atlas);
    assert(static_cast<uint32_t>(glyph.bitmap.glyphSize.x) <= atlas->getWidth());
    assert(static_cast<uint32_t>(glyph.bitmap.glyphSize.y) <= atlas->getHeight());

    IGlyphAtlas::GlyphInfo glyphInfo;
    if (!atlas->tryInsert(glyph.bitmap, glyphInfo)) {
        atlas = addAtlas();
        assert(atlas);
        if (!atlas->tryInsert(glyph.bitmap, glyphInfo)) {
            assert(false);
        }
    }
    _version++;
    return glyph;
}

IGlyphAtlas *GlyphAtlasCache::addAtlas() {
    std::unique_ptr<SkylineGlyphAtlas> atlas =
        std::make_unique<SkylineGlyphAtlas>(_atlasConfig.width, _atlasConfig.height);
    IGlyphAtlas *result = atlas.get();
    _atlases.push_back(std::move(atlas));
    return result;
}

IGlyphAtlas *GlyphAtlasCache::getCurrentAtlas() const {
    assert(_atlases.size() > 0);
    return _atlases[_atlases.size() - 1].get();
}
} // namespace rendell_text