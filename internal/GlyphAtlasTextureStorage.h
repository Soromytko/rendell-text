#pragma once
#include <GlyphAtlasTexture.h>
#include <rendell/oop/Texture2DArray.h>
#include <rendell_text/GlyphAtlasCache.h>

#include <unordered_map>

namespace rendell_text {
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
    getOrCreateAtlasTexture(std::shared_ptr<GlyphAtlasCache> glyphAtlasCache);

private:
    std::unordered_map<size_t, std::weak_ptr<GlyphAtlasTexture>> _textures;
};
} // namespace rendell_text