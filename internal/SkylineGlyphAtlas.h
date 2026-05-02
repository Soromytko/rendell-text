#pragma once
#include <rendell_text/IGlyphAtlas.h>

#include <GlyphKey.h>

#include <optional>
#include <unordered_map>
#include <vector>

namespace rendell_text {
class SkylineGlyphAtlas final : public IGlyphAtlas {
public:
    SkylineGlyphAtlas(Size size);
    ~SkylineGlyphAtlas() = default;

    Size getSize() const override { return _size; }

    bool contains(GlyphKey key) const override { return _glyphs.contains(key); }

    Info getGlyphInfo(GlyphKey key) const override;
    bool findGlyphInfo(GlyphKey key, Info &info) const override;
    PixelsRef getGlyphPixels(GlyphKey key) const override;

    PixelsRef getPixels() const override { return _pixels; }

    bool resize(Size size) override;
    bool insert(const RasterizedGlyph &glyph, FontInstance fontInstance) override;

private:
    struct UV {
        float u0, v0;
        float u1, v1;
    };

    struct GlyphData {
        UV uv;
        RasterizedGlyph glyph;
    };

    struct SkylineNode {
        Size::Type x;
        Size::Type y;
        Size::Type width;
    };

    inline Info makeInfo(UV uv, const RasterizedGlyph &glyph) const;

    int findBestNodeIndex(Size size, Size::Type &yOffset) const;
    std::optional<UV> insertGlyph(const GlyphBitmap &bitmap, size_t nodeInsertIndex);
    void addSkylineSegment(Size::Type index, Size size, Size::Type yOffset);

    Size _size;
    size_t _currentAtlasIndex{};
    std::unordered_map<GlyphKey, GlyphData> _glyphs{};
    std::vector<SkylineNode> _skyline{};
    std::vector<std::byte> _pixels{};
};
} // namespace rendell_text
