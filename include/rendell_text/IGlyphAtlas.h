#pragma once
#include <rendell_text/RasterizedGlyph.h>
#include <rendell_text/types.h>

namespace rendell_text {
class IGlyphAtlas {
public:
    struct Info {
        float u0, u1;
        float v0, v1;
        float bearingX, bearingY;
        Size size;
    };

    IGlyphAtlas() = default;
    virtual ~IGlyphAtlas() = default;

    virtual Size getSize() const = 0;
    virtual bool contains(GlyphKey key) const = 0;
    virtual Info getGlyphInfo(GlyphKey key) const = 0;
    virtual bool findGlyphInfo(GlyphKey key, Info &info) const = 0;
    virtual PixelsRef getGlyphPixels(GlyphKey key) const = 0;
    virtual PixelsRef getPixels() const = 0;

    virtual bool resize(Size size) = 0;
    virtual bool insert(const RasterizedGlyph &rasterizedGlyph, FontInstance fontInstance) = 0;
};
} // namespace rendell_text