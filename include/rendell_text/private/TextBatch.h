#pragma once
#include "RasterizedGlyphRange.h"
#include "TextBuffer.h"

#include <rendell/oop/raii.h>
#include <rendell/oop/rendell_oop.h>
#include <rendell/rendell.h>

#include <glm/glm.hpp>
#include <memory>

namespace rendell_text {
struct TextBatch {
public:
    TextBatch(RasterizedGlyphRangeSharedPtr rasterizedGlyphRange,
              rendell::oop::Texture2DArraySharedPtr textures, size_t textBufferSize);
    ~TextBatch() = default;

    size_t getTextBufferCount() const;

    void beginUpdating();
    void appendCharacter(Char character, glm::vec2 offset);
    void endUpdating();

    void useTexture(rendell::UniformSampler2DId samplerId, uint32_t stage);
    void useTextBuffer(size_t textBufferIndex, uint32_t textBufferBinding,
                       uint32_t transformBufferBinding);

    const RasterizedGlyphRange *getRasterizedGlyphRange() const;
    const rendell::oop::Texture2DArray *getTextures() const;
    const std::vector<TextBufferUniquePtr> &getTextBuffers() const;

private:
    size_t _counter{};
    size_t _textBufferSize{};

    RasterizedGlyphRangeSharedPtr _rasterizedGlyphRange{};
    rendell::oop::Texture2DArraySharedPtr _textures{};
    std::vector<TextBufferUniquePtr> _textBuffers{};
};

RENDELL_USE_RAII_FACTORY(TextBatch)
} // namespace rendell_text
