#include <SkylineGlyphAtlas.h>

#include <algorithm>
#include <cassert>

namespace rendell_text {
SkylineGlyphAtlas::SkylineGlyphAtlas(AtlasType type, Size size)
    : _type(type)
    , _size(size)
    , _skyline({size.width, size.height}) {
    assert(size.width > 0 && size.height > 0);
    _pixels.resize(_size.area() * bytesPerPixel(_type));
}

IGlyphAtlas::Info SkylineGlyphAtlas::getGlyphInfo(GlyphKey key) const {
    const auto it = _glyphs.find(key);
    assert(it != _glyphs.end());
    return makeInfo(it->second.uv, it->second.glyph);
}

bool SkylineGlyphAtlas::findGlyphInfo(GlyphKey key, Info &info) const {
    const auto it = _glyphs.find(key);
    if (it == _glyphs.end()) {
        return false;
    }
    info = makeInfo(it->second.uv, it->second.glyph);
    return true;
}

PixelsRef SkylineGlyphAtlas::getGlyphPixels(GlyphKey key) const {
    const auto it = _glyphs.find(key);
    assert(it != _glyphs.end());
    return it->second.glyph.bitmap.pixels;
}

bool SkylineGlyphAtlas::resize(Size size) {
    assert(false);
    return false;
}

bool SkylineGlyphAtlas::insert(const RasterizedGlyph &glyph, FontInstance fontInstance) {
    if (_type != glyph.atlasType) {
        return false;
    }

    if (glyph.bitmap.size.width > _size.width || glyph.bitmap.size.height > _size.height) {
        return false;
    }

    const auto maybeInsertResult = _skyline.insert(glyph.bitmap.size.width, glyph.bitmap.size.height);
    if (!maybeInsertResult) {
        return false;
    }
    const auto insertResult = *maybeInsertResult;
    auto maybeUV = insertGlyph(static_cast<Size::Type>(insertResult.x),
                               static_cast<Size::Type>(insertResult.y), glyph.bitmap);
    if (!maybeUV) {
        return false;
    }
    _glyphs.insert({GlyphKey{
                        .id = glyph.id,
                        .fontInstance = fontInstance,
                    },
                    GlyphData{
                        .uv = maybeUV.value(),
                        .glyph = glyph,
                    }});
    return true;
}

SkylineGlyphAtlas::Info SkylineGlyphAtlas::makeInfo(UV uv, const RasterizedGlyph &glyph) const {
    return Info{
        .u0 = uv.u0,
        .u1 = uv.u1,
        .v0 = uv.v0,
        .v1 = uv.v1,
        .bearingX = glyph.bearingX,
        .bearingY = glyph.bearingY,
        .size = glyph.bitmap.size,
    };
}

std::optional<SkylineGlyphAtlas::UV> SkylineGlyphAtlas::insertGlyph(Size::Type x, Size::Type y,
                                                                    const GlyphBitmap &bitmap) {
    assert(x + bitmap.size.width <= _size.width);
    assert(y + bitmap.size.height <= _size.height);

    const auto pixelSize = bytesPerPixel(_type);

    for (size_t rowIndex = 0; rowIndex < bitmap.size.height; rowIndex++) {
        const size_t atlasIndex = (x + (y + rowIndex) * _size.width) * pixelSize;
        const size_t bitmapIndex = rowIndex * bitmap.size.width * pixelSize;
        std::byte *dstData = _pixels.data() + atlasIndex;
        const std::byte *srcData = bitmap.pixels.data() + bitmapIndex;
        memcpy(dstData, srcData, bitmap.size.width * pixelSize);
    }

    // for (size_t y = 0; y < bitmap.size.height; y++) {
    //     const size_t atlasIndex = ((y + y) * _size.width + x) * pixelSize;
    //     const size_t glyphIndex = y * bitmap.size.width * pixelSize;
    //     memcpy(&_pixels[atlasIndex], &bitmap.pixels[glyphIndex], bitmap.size.width * pixelSize);
    // }

    return UV{
        .u0 = static_cast<float>(x) / _size.width,
        .v0 = static_cast<float>(y) / _size.height,
        .u1 = static_cast<float>(x + bitmap.size.width) / _size.width,
        .v1 = static_cast<float>(y + bitmap.size.height) / _size.height,
    };
}

} // namespace rendell_text