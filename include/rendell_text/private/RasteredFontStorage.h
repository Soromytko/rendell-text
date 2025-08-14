#pragma once
#include <map>
#include <memory>
#include <rendell_text/defines.h>
#include <rendell_text/private/GlyphBuffer.h>
#include <rendell_text/private/IFontRaster.h>
#include <string>

namespace rendell_text {
class RasteredFontStorage {
public:
    RasteredFontStorage(IFontRasterSharedPtr fontRaster, wchar_t charRangeSize);
    ~RasteredFontStorage() = default;

    void clearCache();
    GlyphBufferSharedPtr rasterizeGlyphRange(wchar_t rangeIndex);

    wchar_t getRangeIndex(wchar_t character) const;
    uint32_t getFontWidth() const;
    uint32_t getFontHeight() const;
    const IFontRasterSharedPtr getFontRaster() const;

private:
    GlyphBufferSharedPtr createGlyphBuffer(wchar_t rangeIndex);

    IFontRasterSharedPtr _fontRaster;
    uint32_t _fontWidth = 64, _fontHeight = 64;
    const wchar_t _charRangeSize;
    std::map<wchar_t, std::shared_ptr<GlyphBuffer>> _cachedGlyphBuffers{};
};

RENDELL_TEXT_DECLARE_SHARED_PTR_FACTORY(RasteredFontStorage)
} // namespace rendell_text
