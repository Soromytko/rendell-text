#pragma once
#include "GlyphBitmap.h"
#include "IFontRaster.h"
#include "IGlyphAtlas.h"

#include <functional>
#include <unordered_map>

namespace rendell_text {
struct Glyph final {
    GlyphBitmap bitmap;
    IGlyphAtlas::GlyphInfo atlasInfo;
};

class GlyphAtlasCache final {
public:
    enum class CallbackType {
        atlasRemoved,
        atlasAdded,
        atlasUpdated,
    };
    using AtlasChangedCallback = std::function<void(CallbackType, size_t, IGlyphAtlas *)>;

    GlyphAtlasCache(IFontRasterSharedPtr fontRaster, AtlasType atlasType = AtlasType::msdf);
    ~GlyphAtlasCache() = default;

    uint32_t getGlyphWidth() const;
    uint32_t getGlyphHeight() const;
    uint32_t getAtlasCount() const;
    const std::vector<std::unique_ptr<IGlyphAtlas>> &getAtlases() const;

    const Glyph &getOrRasterizeGlyph(Codepoint character);
    uint32_t getFontHeight() const;

    void setAtlasChangedCallback(AtlasChangedCallback callback);

private:
    IGlyphAtlas *createAtlas();
    IGlyphAtlas *getCurrentAtlas() const;

    IFontRasterSharedPtr _fontRaster;
    AtlasType _atlasType;

    std::unordered_map<Codepoint, Glyph> _glyphs{};
    std::vector<std::unique_ptr<IGlyphAtlas>> _atlases{};
};
} // namespace rendell_text