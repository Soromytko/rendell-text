#pragma once
#include "Font.h"
#include "GlyphBitmap.h"

#include <rendell/oop/raii.h>

#include <memory>

namespace rendell_text {
class GlyphAtlasCache final {
public:
    GlyphAtlasCache(FontSharedPtr font, AtlasType atlasType = AtlasType::msdf);
    ~GlyphAtlasCache() = default;

    FontSharedPtr getFont() const;
    const GlyphBitmap &getGlyphBitmap() const;
    uint32_t getFontHeight() const;

private:
    FontSharedPtr _font;
    AtlasType _atlasType;
};

RENDELL_USE_RAII_FACTORY(GlyphAtlasCache)
} // namespace rendell_text