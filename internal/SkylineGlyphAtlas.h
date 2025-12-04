#pragma once
#pragma once
#include <rendell_text/IGlyphAtlas.h>

namespace rendell_text {
class SkylineGlyphAtlas final : public IGlyphAtlas {
public:
    SkylineGlyphAtlas(uint32_t width, uint32_t height);
    ~SkylineGlyphAtlas() = default;

    size_t getVersion() const override;
    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    const std::vector<std::byte> &getPixels() const override;
    bool tryInsert(const GlyphBitmap &glyph, GlyphInfo &glyphInfo) override;

private:
    struct SkylineNode final {
        uint32_t x;
        uint32_t y;
        uint32_t width;
    };

    int findBestNodeIndex(const GlyphBitmap &glyph) const;
    void insertGlyph(const GlyphBitmap &glyph, size_t nodeInsertIndex);
    void addSkylineSegment(size_t index, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    size_t _version{};
    uint32_t _width{};
    uint32_t _height{};
    std::vector<SkylineNode> _skyline{};
    std::vector<std::byte> _pixels{};
};
} // namespace rendell_text
