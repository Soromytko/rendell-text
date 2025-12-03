#pragma once
#include <memory>
#include <unordered_map>

namespace rendell_text {
class IGlyphAtlasCache;
class GlyphAtlasTexture;

class GlyphAtlasTextureStorage final {
public:
    static bool init();
    static void release();
    static GlyphAtlasTextureStorage *getInstance();

private:
    GlyphAtlasTextureStorage() = default;

public:
    ~GlyphAtlasTextureStorage() = default;

    std::shared_ptr<GlyphAtlasTexture>
    getOrCreateAtlasTexture(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache);

private:
    std::unordered_map<size_t, std::weak_ptr<GlyphAtlasTexture>> _textures;
};
} // namespace rendell_text