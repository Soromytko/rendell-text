#pragma once
#include "FontRasterizationResult.h"

#include <rendell/oop/raii.h>

#include <filesystem>
#include <memory>

namespace rendell_text {
class IFontRaster {
protected:
    IFontRaster() = default;

public:
    virtual ~IFontRaster() = default;

public:
    virtual bool isInitialized() const = 0;
    virtual const std::filesystem::path &getFontPath() const = 0;
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual int getFontHeight() const = 0;
    virtual int getAscender() const = 0;
    virtual int getDescender() const = 0;

    virtual bool loadFont(const std::filesystem::path &fontPath, uint32_t width,
                          uint32_t height) = 0;

    virtual RasterizedGlyphListSharedPtr rasterize(wchar_t from, wchar_t to) = 0;
};

RENDELL_USE_RAII(IFontRaster)
} // namespace rendell_text
