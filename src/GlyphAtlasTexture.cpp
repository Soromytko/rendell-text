#include <GlyphAtlasTexture.h>

#include <cassert>

namespace rendell_text {
GlyphAtlasTexture::GlyphAtlasTexture(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    setGlyphAtlasCache(glyphAtlasCache);
}

void GlyphAtlasTexture::prepare() {
    assert(_glyphAtlasCache);
    assert(_texture);

    if (_needsFullUpdate) {
        recreateTextureArray();
        _needsFullUpdate = false;
        _version = _glyphAtlasCache->getVersion();
        return;
    }

    if (_version == _glyphAtlasCache->getVersion()) {
        return;
    }

    const auto &atlases = _glyphAtlasCache->getAtlases();
    assert(atlases.size() > 0);
    if (atlases.size() != _atlasVersions.size()) {
        recreateTextureArray();
        _version = _glyphAtlasCache->getVersion();
        _atlasVersions = _glyphAtlasCache->getAtlasVersions();
        return;
    }
    assert(_texture);

    for (size_t i = 0; i < _atlasVersions.size(); i++) {
        const auto &atlas = atlases[i];
        if (_atlasVersions[i] != atlas->getVersion()) {
            _texture->setSubData(static_cast<uint32_t>(i), atlas->getWidth(), atlas->getHeight(),
                                 atlas->getPixels().data());
            _atlasVersions[i] = atlas->getVersion();
        }
    }
    _version = _glyphAtlasCache->getVersion();
}

void GlyphAtlasTexture::use(rendell::UniformSampler2DId uniformId, uint32_t stage) {
    assert(rendell::isValid(uniformId));
    assert(_texture);
    _texture->use(uniformId, stage);
}

void GlyphAtlasTexture::setGlyphAtlasCache(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    if (_glyphAtlasCache != glyphAtlasCache) {
        _glyphAtlasCache = glyphAtlasCache;
        _needsFullUpdate = true;
    }
}

void GlyphAtlasTexture::recreateTextureArray() {
    assert(_glyphAtlasCache);
    const auto &atlases = _glyphAtlasCache->getAtlases();
    if (atlases.size() == 0) {
        _texture.reset();
        return;
    }

    const uint32_t glyphWidth = _glyphAtlasCache->getGlyphWidth();
    const uint32_t glyphHeight = _glyphAtlasCache->getGlyphHeight();

    _texture = std::make_shared<rendell::oop::Texture2DArray>(glyphWidth, glyphHeight,
                                                              static_cast<uint32_t>(atlases.size()),
                                                              rendell::TextureFormat::BGR);
    for (size_t i = 0; i < atlases.size(); i++) {
        const auto &atlas = atlases[i];
        _texture->setSubData(static_cast<uint32_t>(i), atlas->getWidth(), atlas->getHeight(),
                             atlas->getPixels().data());
    }
}

} // namespace rendell_text