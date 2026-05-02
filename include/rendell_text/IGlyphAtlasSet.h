#pragma once
#include <rendell_text/RasterizedGlyph.h>
#include <rendell_text/types.h>

#include <cstdint>

namespace rendell_text {
class IGlyphMultiAtlas {
public:
    struct Info {
        float u0, u1;
        float v0, v1;
        float bearingX, bearingY;
        float advance;
        Size size;
        uint16_t index;
    };

    IGlyphMultiAtlas() = default;
    virtual ~IGlyphMultiAtlas() = default;

    virtual Size getSize() const = 0;
    virtual Size::Type getAtlasCount() const = 0;
    virtual bool contains(GlyphKey key) const = 0;
    virtual Info getGlyphInfo(GlyphKey key) const = 0;
    virtual bool findGlyphInfo(GlyphKey key, Info &result) const = 0;
    virtual PixelsRef getGlyphPixels(GlyphKey key) const = 0;
    virtual PixelsRef getAtlasPixels(Size::Type atlasIndex) const = 0;
    virtual std::vector<PixelsRef> getAtlasesPixels() const = 0;
    virtual PixelsRef getPixels() const = 0;

    virtual bool resize(Size size) = 0;
    virtual bool insert(const RasterizedGlyph &glyph, FontInstance fontInstance) = 0;
};
} // namespace rendell_text