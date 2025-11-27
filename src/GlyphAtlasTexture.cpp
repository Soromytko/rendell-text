#include <GlyphAtlasTexture.h>

#include <cassert>

namespace rendell_text {
GlyphAtlasTexture::GlyphAtlasTexture(std::shared_ptr<GlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    setGlyphAtlasCache(glyphAtlasCache);
}

void GlyphAtlasTexture::prepare() {
    assert(_glyphAtlasCache);
    assert(_texture);

    const uint32_t glyphWidth = _glyphAtlasCache->getGlyphWidth();
    const uint32_t glyphHeight = _glyphAtlasCache->getGlyphHeight();

    if (_needResizeTexture) {
        const auto &atlases = _glyphAtlasCache->getAtlases();
        if (atlases.size() == 0) {
            _texture.reset();
            _dirtyFlags.clear();
            return;
        }
        _texture = std::make_shared<rendell::oop::Texture2DArray>(
            glyphWidth, glyphHeight, atlases.size(), rendell::TextureFormat::BGR);
        for (size_t i = 0; i < atlases.size(); i++) {
            const auto &atlas = atlases[i];
            _texture->setSubData(static_cast<uint32_t>(i), atlas->getWidth(), atlas->getHeight(),
                                 atlas->getPixels().data());
        }
        _dirtyFlags.clear();
        return;
    }

    if (_dirtyFlags.size() == 0) {
        return;
    }

    const auto &atlases = _glyphAtlasCache->getAtlases();
    assert(atlases.size() > 0);
    for (auto it = _dirtyFlags.begin(); it != _dirtyFlags.end(); it++) {
        const size_t dirtyIndex = *it;
        assert(dirtyIndex < atlases.size());
        const auto &atlas = atlases[dirtyIndex];
        _texture->setSubData(static_cast<uint32_t>(dirtyIndex), atlas->getWidth(),
                             atlas->getHeight(), atlas->getPixels().data());
    }
    _dirtyFlags.clear();
}

void GlyphAtlasTexture::setGlyphAtlasCache(std::shared_ptr<GlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    if (_glyphAtlasCache != glyphAtlasCache) {
        _glyphAtlasCache = glyphAtlasCache;
        _glyphAtlasCache->setAtlasChangedCallback(
            [this](GlyphAtlasCache::CallbackType action, size_t index, IGlyphAtlas *atlas) {
                assert(atlas);
                if (action == GlyphAtlasCache::CallbackType::atlasRemoved ||
                    action == GlyphAtlasCache::CallbackType::atlasAdded) {
                    _needResizeTexture = true;
                    _dirtyFlags.clear();
                    return;
                }

                assert(action == GlyphAtlasCache::CallbackType::atlasUpdated);
                _dirtyFlags.insert(index);
            });

        _needResizeTexture = true;
    }
}

} // namespace rendell_text