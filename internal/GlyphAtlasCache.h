#pragma once
#include <rendell_text/IFontRaster.h>
#include <rendell_text/IGlyphAtlasCache.h>

#include <unordered_map>

namespace rendell_text {
class GlyphAtlasCache final : public IGlyphAtlasCache {
public:
    GlyphAtlasCache(IFontRasterSharedPtr fontRaster, AtlasType atlasType = AtlasType::msdf);
    ~GlyphAtlasCache() = default;

    uint32_t getGlyphWidth() const override;
    uint32_t getGlyphHeight() const override;
    uint32_t getAtlasCount() const override;
    uint32_t getFontHeight() const override;
    std::vector<size_t> getAtlasVersions() const override;
    const std::vector<std::unique_ptr<IGlyphAtlas>> &getAtlases() const override;

    const Glyph &getOrRasterizeGlyph(Codepoint character) override;

private:
    IGlyphAtlas *addAtlas();
    IGlyphAtlas *getCurrentAtlas() const;

    IFontRasterSharedPtr _fontRaster;
    AtlasType _atlasType;

    std::unordered_map<Codepoint, Glyph> _glyphs{};
    std::vector<std::unique_ptr<IGlyphAtlas>> _atlases{};
};
} // namespace rendell_text