#pragma once
#include "GlyphBitmap.h"

#include <memory>

namespace rendell_text {
class IFontRaster;
class IGlyphAtlasCache;
class ITextLayout;
class ITextRenderer;

std::shared_ptr<IFontRaster> createFontRaster();
std::shared_ptr<IGlyphAtlasCache> createGlyphAtlasCache(std::shared_ptr<IFontRaster> fontRaster,
                                                        AtlasConfig atlasConfig = {});
std::shared_ptr<ITextLayout> createTextLayout(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache);
std::shared_ptr<ITextRenderer> createTextRenderer(std::shared_ptr<ITextLayout> textLayout);
} // namespace rendell_text