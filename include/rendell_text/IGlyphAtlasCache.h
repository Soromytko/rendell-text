#pragma once
#include <rendell_text/GlyphBitmap.h>
#include <rendell_text/IGlyphAtlas.h>

#include <functional>
#include <memory>

namespace rendell_text {
struct Glyph final {
    GlyphBitmap bitmap;
    IGlyphAtlas::GlyphInfo atlasInfo;
};
} // namespace rendell_text

namespace rendell_text {
class IGlyphAtlasCache {
public:
    enum class CallbackType {
        atlasRemoved,
        atlasAdded,
        atlasUpdated,
    };
    using AtlasChangedCallback = std::function<void(CallbackType, size_t, IGlyphAtlas *)>;

    IGlyphAtlasCache() = default;
    virtual ~IGlyphAtlasCache() = default;

    virtual uint32_t getGlyphWidth() const = 0;
    virtual uint32_t getGlyphHeight() const = 0;
    virtual uint32_t getAtlasCount() const = 0;
    virtual uint32_t getFontHeight() const = 0;
    virtual const std::vector<std::unique_ptr<IGlyphAtlas>> &getAtlases() const = 0;

    virtual void setAtlasChangedCallback(AtlasChangedCallback callback) = 0;

    virtual const Glyph &getOrRasterizeGlyph(Codepoint character) = 0;
};
} // namespace rendell_text