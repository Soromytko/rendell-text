#pragma once
#include <rendell_text/GlyphBitmap.h>
#include <rendell_text/IGlyphAtlas.h>

#include <memory>

namespace rendell_text {
struct Glyph final {
    GlyphBitmap bitmap{};
    size_t atlasIndex{};
    IGlyphAtlas::UV uv{};
};
} // namespace rendell_text

namespace rendell_text {
class IGlyphAtlasCache {
public:
    IGlyphAtlasCache() = default;
    virtual ~IGlyphAtlasCache() = default;

    virtual size_t getVersion() const = 0;
    virtual uint32_t getGlyphWidth() const = 0;
    virtual uint32_t getGlyphHeight() const = 0;
    virtual uint32_t getAtlasCount() const = 0;
    virtual uint32_t getLineHeight() const = 0;
    virtual uint32_t getAscender() const = 0;
    virtual uint32_t getDescender() const = 0;
    virtual std::vector<size_t> getAtlasVersions() const = 0;
    virtual const std::vector<std::unique_ptr<IGlyphAtlas>> &getAtlases() const = 0;

    virtual const Glyph &getOrRasterizeGlyph(Codepoint character) = 0;
};
} // namespace rendell_text