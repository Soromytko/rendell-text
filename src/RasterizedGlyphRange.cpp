#include <rendell_text/private/RasterizedGlyphRange.h>

#include <cassert>

namespace rendell_text {
RasterizedGlyphRange::RasterizedGlyphRange(Char from, Char to,
                                           std::vector<RasterizedGlyph> &&glyphs) {
    assert(from >= 0);
    assert(to > from);
    assert(glyphs.size() == static_cast<size_t>(to - from));
    _from = from;
    _to = to;
    _glyphs = std::move(glyphs);
}

size_t RasterizedGlyphRange::getLength() const {
    return _glyphs.size();
}

const RasterizedGlyph &RasterizedGlyphRange::at(size_t index) const {
    assert(index < _glyphs.size());
    return _glyphs[index];
}

const RasterizedGlyph &RasterizedGlyphRange::getGlyphByChar(Char character) const {
    assert(character >= _from);
    assert(character <= _to);
    const size_t index = static_cast<size_t>(character - _from);
    return _glyphs[index];
}

const RasterizedGlyph &RasterizedGlyphRange::operator[](size_t index) const {
    assert(index < _glyphs.size());
    return _glyphs[index];
}
} // namespace rendell_text
