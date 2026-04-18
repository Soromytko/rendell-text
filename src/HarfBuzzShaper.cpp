#include <HarfBuzzShaper.h>

#include <FontLibrary.h>
#include <FreetypeFontData.h>

#include <cassert>

namespace rendell_text {
void HarfBuzzShaper::configure(Config config) {
}

ShapeResult HarfBuzzShaper::shape(const TextRun &textRun,
                                  IFontFallbackResolver &fontFallbackResolver) {
    IFontData *fontData;
    auto lock = FontLibrary::getInstance()->readAndLock({}, fontData);
    assert(dynamic_cast<FreetypeFontData *>(fontData) != nullptr);
    auto freetypeFontData = static_cast<FreetypeFontData *>(fontData);
}

} // namespace rendell_text