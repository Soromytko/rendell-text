#pragma once
#include <rendell_text/types.h>
#include <rendell_text/RasterizedGlyph.h>

#include <cstdint>
#include <optional>

namespace rendell_text {
class IGlyphMultiAtlas {
public:
    struct Info {
        float x1, x2;
        float y1, y2;
        float bearingX, bearingY;
        Size size;
        uint16_t index;
    };

    IGlyphMultiAtlas() = default;
    virtual ~IGlyphMultiAtlas() = default;

    virtual Size getSize() const = 0;
    virtual Size::Type getAtlasCount() const = 0;
    virtual bool contains(GlyphKey key) const = 0;
    virtual Info getGlyphInfo(GlyphKey key) const = 0;
    virtual std::optional<Info> findGlyphInfo(GlyphKey key) const = 0;
    virtual PixelsRef getGlyphPixels(GlyphKey key) const = 0;
    virtual PixelsRef getAtlasPixels(Size::Type atlasIndex) const = 0;
    virtual std::vector<PixelsRef> getAtlasesPixels() const = 0;
    virtual PixelsRef getPixels() const = 0;

    virtual bool resize(Size size) = 0;
    virtual bool insert(const RasterizedGlyph &glyph, FontInstance fontInstance) = 0;
};
} // namespace rendell_text