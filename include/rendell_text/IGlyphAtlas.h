#pragma once
#include "GlyphBitmap.h"

#include <rendell/DataType.h>

#include <vector>

namespace rendell_text {
class IGlyphAtlas {
public:
    struct UV final {
        float u0, u1;
        float v0, v1;
    };

    virtual size_t getVersion() const = 0;
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual const std::vector<rendell::byte_t> &getPixels() const = 0;

    virtual bool tryInsert(const GlyphBitmap &glyph, UV &uv) = 0;
};
} // namespace rendell_text
