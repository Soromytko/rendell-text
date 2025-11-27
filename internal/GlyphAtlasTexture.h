#pragma once
#pragma once
#include <rendell/oop/Texture2DArray.h>
#include <rendell_text/GlyphAtlasCache.h>

#include <memory>
#include <unordered_set>

namespace rendell_text {
class GlyphAtlasTexture final {
public:
    GlyphAtlasTexture(std::shared_ptr<GlyphAtlasCache> glyphAtlasCache);
    ~GlyphAtlasTexture() = default;

    void prepare();
#error Implement use!
    void use();

private:
    void setGlyphAtlasCache(std::shared_ptr<GlyphAtlasCache> glyphAtlasCache);

    std::shared_ptr<rendell::oop::Texture2DArray> _texture{};
    std::shared_ptr<GlyphAtlasCache> _glyphAtlasCache{};
    std::unordered_set<size_t> _dirtyFlags{};

    bool _needResizeTexture{};
};
} // namespace rendell_text