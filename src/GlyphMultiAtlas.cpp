#include <GlyphMultiAtlas.h>

#include <algorithm>
#include <cassert>
#include <logging.h>

namespace rendell_text {
GlyphMultiAtlas::GlyphMultiAtlas(Size size, Size::Type maxAtlasCount)
    : _size(size)
    , _maxAtlasCount(maxAtlasCount) {
    assert(_size.height > 0 && _size.width > 0);
    assert(_maxAtlasCount > 0);
    addAtlas();
    assert(_atlases.size() > 0);
}

GlyphMultiAtlas::Info GlyphMultiAtlas::getGlyphInfo(GlyphKey key) const {
    Info info;
    const bool suc = findGlyphInfo(key, info);
    assert(suc);
    return info;
}

bool GlyphMultiAtlas::findGlyphInfo(GlyphKey key, Info &result) const {
    for (size_t i = 0; i < _atlases.size(); i++) {
        IGlyphAtlas::Info info;
        if (_atlases[i].findGlyphInfo(key, info)) {
            result = Info{
                .u0 = info.u0,
                .u1 = info.u1,
                .v0 = info.v0,
                .v1 = info.v1,
                .bearingX = info.bearingX,
                .bearingY = info.bearingY,
                .advance = info.advance,
                .size = info.size,
                .index = static_cast<decltype(Info::index)>(i),
            };
            return true;
        }
    }

    return false;
}

PixelsRef GlyphMultiAtlas::getGlyphPixels(GlyphKey key) const {
    const auto atlas = findAtlas(key);
    assert(atlas);
    return atlas->getPixels();
}

PixelsRef GlyphMultiAtlas::getAtlasPixels(Size::Type atlasIndex) const {
    assert(atlasIndex < _atlases.size());
    return _atlases[atlasIndex].getPixels();
}

std::vector<PixelsRef> GlyphMultiAtlas::getAtlasesPixels() const {
    std::vector<PixelsRef> result{_atlases.size()};
    std::transform(_atlases.begin(), _atlases.end(), result.begin(),
                   [](const SkylineGlyphAtlas &atlas) { return atlas.getPixels(); });
    return result;
}

PixelsRef GlyphMultiAtlas::getPixels() const {
    if (_shouldPixelsCacheBeUpdated) {
        _pixelsCache.resize(_size.width * _size.height * sizeof(std::byte));
        auto it = _pixelsCache.begin();
        for (const SkylineGlyphAtlas &atlas : _atlases) {
            assert(atlas.getSize().width == _size.width && atlas.getSize().height == _size.height);
            const auto atlasPixels = atlas.getPixels();
            std::copy(atlasPixels.begin(), atlasPixels.end(), it);
            it += atlasPixels.size();
        }
        _shouldPixelsCacheBeUpdated = false;
    }
    return _pixelsCache;
}

bool GlyphMultiAtlas::resize(Size size) {
    return false;
}

bool GlyphMultiAtlas::insert(const RasterizedGlyph &glyph, FontInstance fontInstance) {
    const auto glyphSize = glyph.bitmap.size;
    if (glyphSize.width >= _size.width || glyphSize.height > _size.height) {
        RT_WARNING("Glyph[ID: {}].size ({}, {}) > Atlas.size ({}, {})",
                   static_cast<size_t>(glyph.id), glyphSize.width, glyphSize.height, _size.width,
                   _size.height);
        return false;
    }

    const GlyphKey key{glyph.id, fontInstance};
    if (findAtlas(key)) {
        RT_WARNING("Atlas already contains Glyph[ID: {}]", static_cast<size_t>(glyph.id));
        return true;
    }

    IGlyphAtlas *atlas = getCurrentAtlas();
    if (!atlas) {
        return false;
    }
    assert(glyph.bitmap.size.width <= atlas->getSize().width);
    assert(glyph.bitmap.size.height <= atlas->getSize().height);

    if (!atlas->insert(glyph, fontInstance)) {
        atlas = addAtlas();
        if (atlas && !atlas->insert(glyph, fontInstance)) {
            assert(false);
        }
    }
    return true;
}

const SkylineGlyphAtlas *GlyphMultiAtlas::findAtlas(GlyphKey key) const {
    for (const SkylineGlyphAtlas &atlas : _atlases) {
        if (atlas.contains(key)) {
            return &atlas;
        }
    }
    return nullptr;
}

size_t GlyphMultiAtlas::getCurrentAtlasIndex() const {
    assert(_atlases.size() > 0);
    return _atlases.size() - 1;
}

SkylineGlyphAtlas *GlyphMultiAtlas::addAtlas() {
    if (_maxAtlasCount <= static_cast<Size::Type>(_atlases.size())) {
        return nullptr;
    }
    _atlases.push_back(SkylineGlyphAtlas({_size.width, _size.height}));
    return &_atlases[_atlases.size() - 1];
}

SkylineGlyphAtlas *GlyphMultiAtlas::getCurrentAtlas() {
    assert(_atlases.size() > 0);
    return &_atlases[getCurrentAtlasIndex()];
}
} // namespace rendell_text