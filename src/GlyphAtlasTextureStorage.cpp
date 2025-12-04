#include <GlyphAtlasTextureStorage.h>

#include <GlyphAtlasTexture.h>
#include <rendell_text/IGlyphAtlasCache.h>

#include <cassert>

namespace rendell_text {
static std::unique_ptr<GlyphAtlasTextureStorage> s_instance{nullptr};

bool GlyphAtlasTextureStorage::init() {
    assert(!s_instance);
    s_instance.reset(new GlyphAtlasTextureStorage());
    return false;
}

void GlyphAtlasTextureStorage::release() {
    assert(s_instance);
    s_instance.release();
}

GlyphAtlasTextureStorage *GlyphAtlasTextureStorage::getInstance() {
    assert(s_instance);
    return s_instance.get();
}

static size_t getKey(IGlyphAtlasCache *glyphAtlasCache) {
    assert(glyphAtlasCache);
    return reinterpret_cast<size_t>(glyphAtlasCache);
}

std::shared_ptr<GlyphAtlasTexture> GlyphAtlasTextureStorage::getOrCreateAtlasTexture(
    std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    const size_t key = getKey(glyphAtlasCache.get());
    auto it = _textures.find(key);
    if (it != _textures.end()) {
        if (auto locked = it->second.lock()) {
            return locked;
        }
    }

    auto texture = std::make_shared<GlyphAtlasTexture>(glyphAtlasCache);
    _textures[key] = texture;
    return texture;
}

} // namespace rendell_text