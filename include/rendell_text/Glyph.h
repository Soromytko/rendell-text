#pragma once

namespace rendell_text {
struct Glyph final {
    GlyphBitmap bitmap{};
    size_t atlasIndex{};
    IGlyphAtlas::UV uv{};
};

struct GlyphView final {
    GlyphId glyphId;
    size_t atlasIndex;
};
} // namespace rendell_text
