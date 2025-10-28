#pragma once
#include "freetype.h"
#include <logging.h>
#include <rendell/oop/raii.h>
#include <rendell_text/private/IFontRaster.h>

namespace rendell_text {
class FontRaster : public IFontRaster {
public:
    FontRaster() = default;
    FontRaster(const std::filesystem::path &fontPath, uint32_t width, uint32_t height);
    ~FontRaster();

    bool isInitialized() const override;
    const std::filesystem::path &getFontPath() const override;
    int getFontHeight() const override;
    int getAscender() const override;
    int getDescender() const override;

    bool loadFont(const std::filesystem::path &fontPath, uint32_t width, uint32_t height) override;

    bool rasterize(wchar_t from, wchar_t to, FontRasterizationResult &result) override;

private:
    bool init();
    void releaseFace();
    bool rasterizeChar(wchar_t character, FT_Glyph &result);
    FT_Glyph rasterizeGlyphStub();

    FT_Face _face{nullptr};
    std::filesystem::path _fontPath{};
    uint32_t _width{24};
    uint32_t _height{24};
};

RENDELL_USE_RAII_FACTORY(FontRaster)
} // namespace rendell_text
