#pragma once
#include "Glyph.h"
#include <rendell_text/GlyphBitmap.h>
#include <rendell_text/IGlyphAtlas.h>
#include <rendell_text/types.h>

#include <cstdint>
#include <memory>

namespace rendell_text {
class IGlyphAtlasCache {
public:
    IGlyphAtlasCache() = default;
    virtual ~IGlyphAtlasCache() = default;

    virtual bool contains(GlyphId glyphId) const = 0;

    virtual uint32_t getGlyphWidth() const = 0;
    virtual uint32_t getGlyphHeight() const = 0;
    virtual uint32_t getAtlasCount() const = 0;
    virtual uint32_t getLineHeight() const = 0;
    virtual uint32_t getAscender() const = 0;
    virtual uint32_t getDescender() const = 0;
    virtual const std::vector<std::unique_ptr<IGlyphAtlas>> &getAtlases() const = 0;

    virtual const Glyph &getOrRasterizeGlyph(Codepoint character) = 0;

    virtual void AddGlyph(GlyphId glyphId) = 0;
};
} // namespace rendell_text