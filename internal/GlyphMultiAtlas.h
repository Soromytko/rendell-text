#pragma once
#include <SkylineGlyphAtlas.h>
#include <rendell_text/IGlyphAtlasSet.h>

namespace rendell_text {
class GlyphMultiAtlas final : public IGlyphMultiAtlas {
public:
    GlyphMultiAtlas(Size size, Size::Type maxAtlasCount);
    ~GlyphMultiAtlas() = default;

    bool contains(GlyphKey key) const override { return findAtlas(key) != nullptr; }

    Size getSize() const override { return _size; }

    Size::Type getAtlasCount() const override { return static_cast<Size::Type>(_atlases.size()); }

    Info getGlyphInfo(GlyphKey key) const override;
    bool findGlyphInfo(GlyphKey key, Info &result) const override;
    PixelsRef getGlyphPixels(GlyphKey key) const override;
    PixelsRef getAtlasPixels(Size::Type atlasIndex) const override;
    std::vector<PixelsRef> getAtlasesPixels() const override;
    PixelsRef getPixels() const override;

    bool resize(Size size) override;
    bool insert(const RasterizedGlyph &glyph, FontInstance fontInstance) override;

private:
    const SkylineGlyphAtlas *findAtlas(GlyphKey key) const;
    size_t getCurrentAtlasIndex() const;
    SkylineGlyphAtlas *addAtlas();
    SkylineGlyphAtlas *getCurrentAtlas();

    Size _size;
    Size::Type _maxAtlasCount;
    std::vector<SkylineGlyphAtlas> _atlases{};
    mutable bool _shouldPixelsCacheBeUpdated{};
    mutable std::vector<std::byte> _pixelsCache{};
};
} // namespace rendell_text