#pragma once
#include <rendell_text/RasterizedGlyph.h>
#include <rendell_text/types.h>

#include <optional>

namespace rendell_text {
class IGlyphAtlas {
public:
    struct Info {
        float x1, x2;
        float y1, y2;
        Size size;
    };

    IGlyphAtlas() = default;
    virtual ~IGlyphAtlas() = default;

    virtual Size getSize() const = 0;
    virtual bool contains(GlyphKey key) const = 0;
    virtual Info getGlyphInfo(GlyphKey key) const = 0;
    virtual std::optional<Info> findGlyphInfo(GlyphKey key) const = 0;
    virtual PixelsRef getGlyphPixels(GlyphKey key) const = 0;
    virtual PixelsRef getPixels() const = 0;

    virtual bool resize(Size size) = 0;
    virtual bool insert(GlyphId glyphId, const GlyphBitmap &bitmap, FontInstance fontInstance) = 0;
};
} // namespace rendell_text