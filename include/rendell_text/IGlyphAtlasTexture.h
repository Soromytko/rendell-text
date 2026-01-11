#pragma once
#include <rendell/ResourceId.h>

namespace rendell_text {
class IGlyphAtlasTexture {
public:
    IGlyphAtlasTexture() = default;
    virtual ~IGlyphAtlasTexture() = default;

    virtual void prepare() = 0;
    virtual void use(rendell::UniformSampler2DId uniformId, uint32_t stage) = 0;
};
} // namespace rendell_text