#pragma once
#include <rendell_text/FontInstance.h>
#include <rendell_text/types.h>

#include <glm/glm.hpp>
#include <vector>

namespace rendell_text {
struct RasterizedGlyph final {
    GlyphId id;
    glm::vec4 uvRect;
    uint16_t width;
    uint16_t height;
    uint16_t layer;
};

struct RasterizedResult final {
    FontInstance fontInstance;
    std::vector<RasterizedGlyph> rasterisedGlyphs;
};
} // namespace rendell_text