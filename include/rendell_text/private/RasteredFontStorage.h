#pragma once
#include "RasterizedGlyphRange.h"
#include <rendell_text/private/IFontRaster.h>

#include <rendell/oop/rendell_oop.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace rendell_text {
class RasteredFontStorage {
public:
    RasteredFontStorage(IFontRasterSharedPtr fontRaster, size_t charRangeSize);
    ~RasteredFontStorage() = default;

    RasterizedGlyphRangeSharedPtr rasterizeGlyphRange(size_t rangeIndex);
    rendell::oop::Texture2DArraySharedPtr getOrCreateGlyphTexture(size_t rangeIndex);

    size_t getRangeIndex(wchar_t character) const;
    uint32_t getFontWidth() const;
    uint32_t getFontHeight() const;
    const IFontRasterSharedPtr getFontRaster() const;

private:
    RasterizedGlyphRangeSharedPtr rasterizeRange(size_t rangeIndex);
    rendell::oop::Texture2DArraySharedPtr createGlyphTexture(size_t rangeIndex);

    IFontRasterSharedPtr _fontRaster;
    uint32_t _fontWidth = 64, _fontHeight = 64;
    const size_t _charRangeSize;
    std::unordered_map<size_t, RasterizedGlyphRangeWeakPtr> _cachedRasterizedGlyphRanges{};
    std::unordered_map<size_t, rendell::oop::Texture2DArrayWeakPtr> _cachedGlyphTextures{};
};

RENDELL_USE_RAII_FACTORY(RasteredFontStorage)
} // namespace rendell_text
