#pragma once
#include "GlyphBitmap.h"

#include <memory>

namespace rendell_text {
class IFontRaster;
class IGlyphAtlasCache;
class ITextLayout;
class ITextRenderer;
class ITextBuffer;
class IGlyphAtlasTexture;
class IGlyphShaper;
class IFontFallbackResolver;
class IGlyphRaster;

std::shared_ptr<IFontRaster> createFontRaster();
std::shared_ptr<IGlyphAtlasCache> createGlyphAtlasCache(std::shared_ptr<IFontRaster> fontRaster,
                                                        AtlasConfig atlasConfig = {});
std::shared_ptr<ITextLayout> createTextLayout(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache);
std::shared_ptr<ITextBuffer> createTextBuffer(std::shared_ptr<ITextLayout> textLayout);
std::shared_ptr<IGlyphAtlasTexture>
createGlyphAtlasTexture(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache);
std::shared_ptr<ITextRenderer> createTextRenderer(std::shared_ptr<ITextBuffer> textBuffer,
                                                  std::shared_ptr<IGlyphAtlasTexture> atlasTexture);
std::unique_ptr<IGlyphShaper> createGlyphShaper();
std::unique_ptr<IFontFallbackResolver> createFontFallbackResolver();
std::unique_ptr<IGlyphRaster> createGlyphRaster();
} // namespace rendell_text