#pragma once
#pragma once
#include <rendell_text/IFontRaster.h>

#include <rendell/oop/raii.h>
#include <rendell_text/GlyphBitmap.h>

#include "freetype.h"

namespace msdfgen {
class FreetypeHandle;
class FontHandle;
} // namespace msdfgen

namespace rendell_text {
class FontRaster final : public IFontRaster {
public:
    FontRaster();
    ~FontRaster() = default;

    const std::filesystem::path &getFontPath() const override;
    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    int getFontHeight() const override;
    int getAscender() const override;
    int getDescender() const override;

    bool setFontData(const std::byte *data, size_t size);
    void setFontSize(uint32_t width, uint32_t height);

    bool rasterizeGlyph(Codepoint character, AtlasType atlasType, GlyphBitmap &result) override;

    bool rasterizeGlyphBitmap(Codepoint character, GlyphBitmap &result);
    bool rasterizeGlyphSDF(Codepoint character, GlyphBitmap &result);
    bool rasterizeGlyphMSDF(Codepoint character, GlyphBitmap &result);
    bool rasterizeGlyphMTSDF(Codepoint character, GlyphBitmap &result);

private:
    bool init();
    bool rasterizeChar(Codepoint character, FT_Glyph &result);
    FT_Glyph rasterizeGlyphStub();

    FT_Library _freetype{nullptr};
    FT_Face _face{nullptr};

    msdfgen::FreetypeHandle *_freetypeHandle{nullptr};
    msdfgen::FontHandle *_fontHandle{nullptr};

    std::filesystem::path _fontPath{};
    uint32_t _width{24};
    uint32_t _height{24};
};

RENDELL_USE_RAII_FACTORY(FontRaster)
} // namespace rendell_text
