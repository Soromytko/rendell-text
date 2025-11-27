#pragma once
#include <rendell_text/IGlyphAtlas.h>

namespace rendell_text {
class SkylineGlyphAtlas final : public IGlyphAtlas {
public:
    SkylineGlyphAtlas(uint32_t width, uint32_t height);
    ~SkylineGlyphAtlas() = default;

    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    const std::vector<std::byte> &getPixels() const override;
    bool tryInsert(const GlyphBitmap &glyph, GlyphInfo &glyphInfo) override;

private:
    uint32_t _width{};
    uint32_t _height{};
    std::vector<std::byte> _pixels{};
};
} // namespace rendell_text
