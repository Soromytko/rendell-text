#pragma once
#include <rendell_text/IFontFallbackResolver.h>
#include <rendell_text/IGlyphShaper.h>

namespace rendell_text {
class GlyphShaper final : public IGlyphShaper {
public:
    GlyphShaper() = default;
    ~GlyphShaper() = default;

    void configure(Config config) override;
    ShapeResult shape(const TextRun &textRun, IFontFallbackResolver &fontFallbackResolver) override;
};
} // namespace rendell_text