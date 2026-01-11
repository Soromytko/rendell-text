#pragma once
#include <rendell/oop/Texture2DArray.h>
#include <rendell_text/IGlyphAtlasTexture.h>

#include <memory>

namespace rendell_text {
class IGlyphAtlasCache;

class GlyphAtlasTexture final : public IGlyphAtlasTexture {
public:
    GlyphAtlasTexture(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache);
    ~GlyphAtlasTexture() = default;

    void prepare() override;
    void use(rendell::UniformSampler2DId uniformId, uint32_t stage) override;

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