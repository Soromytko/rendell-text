#pragma once
#include "FontRasterizationResult.h"

#include <rendell/oop/rendell_oop.h>
#include <rendell/rendell.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace rendell_text {
class TextBuffer {
public:
    TextBuffer(size_t length);
    ~TextBuffer() = default;

    void beginUpdating();
    void appendCharacter(const RasterizedChar &rasterizedChar, glm::vec2 offset);
    void insertCharacter(const RasterizedChar &rasterizedChar, glm::vec2 offset, size_t index);
    void endUpdating();
    void updateBufferSubData(size_t from, size_t to);

    bool isFull() const;
    void use(uint32_t textBufferBinding, uint32_t transformBufferBinding) const;

    size_t getLength() const;
    size_t getCurrentLength() const;

private:
    size_t _length{};
    size_t _counter{};

    std::vector<uint32_t> _textBufferData{};
    std::vector<glm::vec4> _transformBufferData{};

    rendell::oop::ShaderBufferSharedPtr _textBuffer{};
    rendell::oop::ShaderBufferSharedPtr _transformBuffer{};
};

RENDELL_USE_RAII_FACTORY(TextBuffer)
} // namespace rendell_text
