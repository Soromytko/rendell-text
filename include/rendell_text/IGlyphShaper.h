#pragma once
#include <rendell_text/FontInstance.h>
#include <rendell_text/ShapeResult.h>
#include <rendell_text/TextRun.h>

#include <cstdint>
#include <vector>

namespace rendell_text {
class IGlyphShaper {
public:
    struct Config {
        bool enableKerning = true;
        bool enableLigatures = true;
    };

    IGlyphShaper() = default;
    virtual ~IGlyphShaper() = default;

    virtual void configure(Config config) = 0;
    virtual ShapeResult shape(const TextRun &textRun,
                              IFontFallbackResolver &fontFallbackResolver) = 0;
};
} // namespace rendell_text
