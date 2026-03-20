#pragma once

namespace rendell_text {
struct Glyph final {
    GlyphBitmap bitmap{};
    size_t atlasIndex{};
    IGlyphAtlas::UV uv{};
};
} // namespace rendell_text
