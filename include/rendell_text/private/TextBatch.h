#pragma once
#include "GlyphBuffer.h"
#include "TextBuffer.h"

#include <rendell/oop/raii.h>

#include <glm/glm.hpp>
#include <memory>

namespace rendell_text {
struct TextBatch {
public:
    TextBatch(GlyphBufferSharedPtr glyphBuffer, size_t textBufferSize);
    ~TextBatch() = default;

    void beginUpdating();
    void appendCharacter(wchar_t character, glm::vec2 offset);
    void endUpdating();

    const GlyphBuffer *getGlyphBuffer() const;
    const std::vector<TextBufferUniquePtr> &getTextBuffers() const;

private:
    size_t _counter{};
    size_t _textBufferSize{};

    GlyphBufferSharedPtr _glyphBuffer{};
    std::vector<TextBufferUniquePtr> _textBuffers{};
};

RENDELL_USE_RAII_FACTORY(TextBatch)
} // namespace rendell_text
