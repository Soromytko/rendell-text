#pragma once
#include "GlyphBitmap.h"

#include <filesystem>

namespace rendell_text {
class IFontRaster {
public:
    IFontRaster() = default;
    virtual ~IFontRaster() = default;

    virtual const std::filesystem::path &getFontPath() const = 0;
    virtual uint32_t getFontWiidth() const = 0;
    virtual uint32_t getGlyphHeight() const = 0;
    virtual uint32_t getLineHeight() const = 0;
    virtual int getAscender() const = 0;
    virtual int getDescender() const = 0;

    virtual bool setFontData(const std::byte *data, size_t size) = 0;
    virtual void setFontSize(uint32_t width, uint32_t height) = 0;
    virtual bool rasterizeGlyph(Codepoint character, AtlasType atlasType, GlyphBitmap &result) = 0;
};
} // namespace rendell_text
