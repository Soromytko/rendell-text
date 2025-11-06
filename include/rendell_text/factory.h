#pragma once
#include <rendell_text/types.h>

#include <rendell_text/ShaperConfig.h>

#include <memory>

namespace rendell_text {
class IGlyphAtlas;
class IGlyphMultiAtlas;
class IGlyphShaper;
class IFontFallbackResolver;
class IGlyphRaster;

std::unique_ptr<IGlyphShaper> createGlyphShaper(ShaperConfig config = {});
std::unique_ptr<IGlyphRaster> createGlyphRaster();
std::unique_ptr<IFontFallbackResolver> createFontFallbackResolver();
std::unique_ptr<IGlyphAtlas> createAtlas(Size size);
std::unique_ptr<IGlyphMultiAtlas> createMultiAtlas(Size size, Size::Type maxAtlasCount = 10);

} // namespace rendell_text