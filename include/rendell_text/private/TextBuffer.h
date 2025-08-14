#pragma once
#include "FontRasterizationResult.h"
#include <glm/glm.hpp>
#include <memory>
#include <rendell/rendell.h>

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
    void bind(uint32_t textBufferBinding, uint32_t transformBufferBinding) const;
    void unbind() const;

    size_t getLength() const;
    size_t getCurrentLength() const;

private:
    size_t _length{};
    size_t _counter{};

    std::vector<uint32_t> _textBufferData{};
    std::vector<glm::vec4> _transformBufferData{};

    rendell::ShaderBufferSharedPtr _textBuffer{};
    rendell::ShaderBufferSharedPtr _transformBuffer{};
};
} // namespace rendell_text
