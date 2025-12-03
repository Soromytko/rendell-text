#pragma once
#pragma once
#include <rendell/oop/Texture2DArray.h>
#include <rendell/rendell.h>
#include <rendell_text/IGlyphAtlasCache.h>

#include <memory>

namespace rendell_text {
class GlyphAtlasTexture final {
public:
    GlyphAtlasTexture(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache);
    ~GlyphAtlasTexture() = default;

    void prepare();
    void use(rendell::UniformSampler2DId uniformId, size_t stage);

private:
    void setGlyphAtlasCache(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache);
    void recreateTextureArray();

    size_t _version{0};
    std::vector<size_t> _atlasVersions{};

    std::shared_ptr<rendell::oop::Texture2DArray> _texture{};
    std::shared_ptr<IGlyphAtlasCache> _glyphAtlasCache{};

    bool _needsFullUpdate{};
};
} // namespace rendell_text