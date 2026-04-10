#pragma once
#include <rendell_text/IGlyphAtlasCache.h>

#include <memory>
#include <unordered_map>

namespace rendell_text {
class IFontRaster;

class GlyphAtlasCache final : public IGlyphAtlasCache {
public:
    GlyphAtlasCache(std::shared_ptr<IFontRaster> fontRaster, AtlasConfig atlasConfig = {});
    ~GlyphAtlasCache() = default;

    bool contains(GlyphId glyphId) const override;
    std::optional<GlyphView>

    uint32_t getGlyphWidth() const override;
    uint32_t getGlyphHeight() const override;
    uint32_t getAtlasCount() const override;
    uint32_t getLineHeight() const override;
    uint32_t getAscender() const override;
    uint32_t getDescender() const override;
    const std::vector<std::unique_ptr<IGlyphAtlas>> &getAtlases() const override;

    const Glyph &getOrRasterizeGlyph(Codepoint character) override;

private:
    size_t getCurrentAtlasIndex() const;
    IGlyphAtlas *addAtlas();
    IGlyphAtlas *getCurrentAtlas() const;

    size_t _version{};

    std::shared_ptr<IFontRaster> _fontRaster;
    AtlasConfig _atlasConfig;

    std::unordered_map<Codepoint, Glyph> _glyphs{};
    std::vector<std::unique_ptr<IGlyphAtlas>> _atlases{};
};
} // namespace rendell_text