#include <rendell_text/factory.h>

#include "FontRaster.h"
#include <FontFallbackResolver.h>
#include <GlyphAtlasCache.h>
#include <GlyphAtlasTexture.h>
#include <GlyphRaster.h>
#include <GlyphShaper.h>
#include <TextBuffer.h>
#include <TextLayout.h>
#include <TextRenderer.h>

namespace rendell_text {
std::shared_ptr<IFontRaster> createFontRaster() {
    return std::make_shared<FontRaster>();
}

std::shared_ptr<IGlyphAtlasCache> createGlyphAtlasCache(std::shared_ptr<IFontRaster> fontRaster,
                                                        AtlasConfig atlasConfig) {
    assert(fontRaster);
    return std::make_shared<GlyphAtlasCache>(fontRaster, atlasConfig);
}

std::shared_ptr<ITextLayout> createTextLayout(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    return std::make_shared<TextLayout>(glyphAtlasCache);
}

std::shared_ptr<ITextBuffer> createTextBuffer(std::shared_ptr<ITextLayout> textLayout) {
    assert(textLayout);
    return std::make_shared<TextBuffer>(textLayout);
}

std::shared_ptr<IGlyphAtlasTexture>
createGlyphAtlasTexture(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    return std::make_shared<GlyphAtlasTexture>(glyphAtlasCache);
}

std::shared_ptr<ITextRenderer>
createTextRenderer(std::shared_ptr<ITextBuffer> textBuffer,
                   std::shared_ptr<IGlyphAtlasTexture> atlasTexture) {
    assert(textBuffer);
    assert(atlasTexture);
    return std::make_shared<TextRenderer>(textBuffer, atlasTexture);
}

std::unique_ptr<IGlyphShaper> createGlyphShaper() {
    return std::make_unique<HarfBuzzShaper>();
}

std::unique_ptr<IFontFallbackResolver> createFontFallbackResolver() {
    return std::make_unique<FontFallbackResolver>();
}

std::unique_ptr<IGlyphRaster> createGlyphRaster() {
    return std::make_unique<GlyphRaster>();
}
} // namespace rendell_text