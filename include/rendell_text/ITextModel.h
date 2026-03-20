#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace rendell_text {
// struct GlyphMetric final {
//     uint32_t width{};
//     uint32_t height{};
//     glm::ivec2 bearing{};
//     uint32_t advance{};
// };

struct GlyphInstance final {
    glm::vec4 rect;
    glm::vec4 uvRect;
    uint32_t layer;
    uint32_t color;
};

class ITextModel {
public:
    ITextModel() = default;
    virtual ~ITextModel() = default;

    virtual bool isEmpty() const = 0;

    // !!!!!!!!
    virtual void insert(const GlyphInstance *glyphs, size_t count, size_t insertIndex) = 0;
    virtual void update(const GlyphInstance *glyphs, size_t count, size_t startIndex) = 0;
    virtual void erase(size_t startIndex, size_t count) = 0;
};

struct GlyphMeta final {
    size_t charIndex;
    float advance;
};

class ITextLogicalModel {};
} // namespace rendell_text
