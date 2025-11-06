#pragma once
#include "RasterizedGlyph.h"
#include <rendell/oop/rendell_oop.h>
#include <rendell/rendell.h>

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <vector>

namespace rendell_text {
class RasterizedGlyphRange final {
public:
    RasterizedGlyphRange(Char from, Char to, std::vector<RasterizedGlyph> &&glyphs);
    ~RasterizedGlyphRange() = default;

    size_t getLength() const;
    const RasterizedGlyph &at(size_t index) const;
    const RasterizedGlyph &getGlyphByChar(Char character) const;

    const RasterizedGlyph &operator[](size_t index) const;

    inline auto begin() const { return _glyphs.begin(); }

    inline auto end() const { return _glyphs.end(); }

private:
    Char _from;
    Char _to;
    std::vector<RasterizedGlyph> _glyphs{};
};
RENDELL_USE_RAII_FACTORY(RasterizedGlyphRange)

} // namespace rendell_text
