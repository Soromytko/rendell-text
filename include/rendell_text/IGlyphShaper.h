#pragma once
#include <rendell_text/ShapedGlyph.h>
#include <rendell_text/TextRun.h>

namespace rendell_text {
class IGlyphShaper {
public:
    IGlyphShaper() = default;
    virtual ~IGlyphShaper() = default;

    virtual ShapeResult shape(const TextRun &textRun) = 0;
    virtual void shape(const TextRun &textRun, ShapeResult &result) = 0;
};
} // namespace rendell_text
