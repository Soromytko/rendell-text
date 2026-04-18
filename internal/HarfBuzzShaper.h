#pragma once
#include <rendell_text/IFontFallbackResolver.h>
#include <rendell_text/IGlyphShaper.h>

namespace rendell_text {
class HarfBuzzShaper final : public IGlyphShaper {
public:
    HarfBuzzShaper() = default;
    ~HarfBuzzShaper() = default;

    void configure(Config config) override;
    ShapeResult shape(const TextRun &textRun, IFontFallbackResolver &fontFallbackResolver) override;
};
} // namespace rendell_text