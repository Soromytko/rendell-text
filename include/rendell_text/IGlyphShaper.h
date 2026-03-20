#pragma once
#include <rendell_text/TextRun.h>

namespace rendell_text {
struct ShapedGlyph {};

class IGlyphShaper {
public:
    IGlyphShaper() = default;
    virtual ~IGlyphShaper() = default;

    virtual std::vector<ShapedGlyph> shape(const TextRun &textRun) = 0;
};
} // namespace rendell_text
