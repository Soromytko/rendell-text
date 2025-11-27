#include <SkylineGlyphAtlas.h>

namespace rendell_text {
SkylineGlyphAtlas::SkylineGlyphAtlas(uint32_t width, uint32_t height)
    : _width(width)
    , _height(height) {
    assert(_width > 0);
    assert(_height > 0);
    _pixels.resize(_width * _height * 3);
}

uint32_t SkylineGlyphAtlas::getWidth() const {
    return _width;
}

uint32_t SkylineGlyphAtlas::getHeight() const {
    return _height;
}

const std::vector<rendell::byte_t> &SkylineGlyphAtlas::getPixels() const {
    return _pixels;
}

bool SkylineGlyphAtlas::tryInsert(const GlyphBitmap &glyph, GlyphInfo &glyphInfo) {
    return false;
}
} // namespace rendell_text