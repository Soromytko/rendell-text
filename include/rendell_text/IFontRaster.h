#pragma once
#include "GlyphBitmap.h"

#include <rendell/oop/raii.h>

#include <filesystem>
#include <memory>

namespace rendell_text {
class IFontRaster {
public:
    IFontRaster() = default;
    virtual ~IFontRaster() = default;

    virtual const std::filesystem::path &getFontPath() const = 0;
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual int getFontHeight() const = 0;
    virtual int getAscender() const = 0;
    virtual int getDescender() const = 0;

    virtual bool rasterizeGlyph(Codepoint character, AtlasType atlasType, GlyphBitmap &result) = 0;
};

RENDELL_USE_RAII(IFontRaster);
} // namespace rendell_text
