#pragma once
#include "GlyphBitmap.h"
#include "IFontRaster.h"

#include <rendell/oop/raii.h>

#include <unordered_map>

namespace rendell_text {
class GlyphAtlasCache final {
public:
    GlyphAtlasCache(IFontRasterSharedPtr fontRaster, AtlasType atlasType = AtlasType::msdf);
    ~GlyphAtlasCache() = default;

    const GlyphBitmap &getGlyphBitmap(Codepoint character) const;
    uint32_t getFontHeight() const;

private:
    IFontRasterSharedPtr _fontRaster;
    AtlasType _atlasType;

    mutable std::unordered_map<Codepoint, GlyphBitmap> _glyphs;
};

RENDELL_USE_RAII_FACTORY(GlyphAtlasCache)
} // namespace rendell_text