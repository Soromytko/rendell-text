#include <rendell_text/factory.h>

#include <FontFallbackResolver.h>
#include <GlyphMultiAtlas.h>
#include <GlyphRaster.h>
#include <HarfBuzzShaper.h>
#include <SkylineGlyphAtlas.h>

namespace rendell_text {
std::unique_ptr<IGlyphShaper> createGlyphShaper(ShaperConfig config) {
    return std::make_unique<HarfBuzzShaper>(config);
}

std::unique_ptr<IGlyphRaster> createGlyphRaster() {
    return std::make_unique<GlyphRaster>();
}

std::unique_ptr<IFontFallbackResolver> createFontFallbackResolver() {
    return std::make_unique<FontFallbackResolver>();
}

std::unique_ptr<IGlyphAtlas> createAtlas(AtlasType type, Size size) {
    return std::make_unique<SkylineGlyphAtlas>(type, size);
}

std::unique_ptr<IGlyphMultiAtlas> createMultiAtlas(AtlasType type, Size size,
                                                   Size::Type maxAtlasCount) {
    return std::make_unique<GlyphMultiAtlas>(type, size, maxAtlasCount);
}
} // namespace rendell_text