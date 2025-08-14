#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <rendell_text/private/TextBatch.h>

namespace rendell_text {
TextBuffer::TextBuffer(size_t length)
    : _length(length) {
    _textBufferData.resize(_length);
    _transformBufferData.resize(_length);

    _textBuffer = rendell::createShaderBuffer(nullptr, _length * sizeof(uint32_t));
    _transformBuffer = rendell::createShaderBuffer(nullptr, _length * sizeof(glm::mat4));
}

void TextBuffer::beginUpdating() {
    _counter = 0;
}

void TextBuffer::appendCharacter(const RasterizedChar &rasterizedChar, glm::vec2 offset) {
    _textBufferData[_counter] = static_cast<uint32_t>(rasterizedChar.character);
    _transformBufferData[_counter] =
        glm::vec4(offset, rasterizedChar.glyphSize.x, rasterizedChar.glyphSize.y);
    _counter++;
}

void TextBuffer::insertCharacter(const RasterizedChar &rasterizedChar, glm::vec2 offset,
                                 size_t index) {
    _textBufferData[index] = static_cast<uint32_t>(rasterizedChar.character);
    _transformBufferData[index] =
        glm::vec4(offset, rasterizedChar.glyphSize.x, rasterizedChar.glyphSize.y);
}

void TextBuffer::endUpdating() {
    _textBuffer->setSubData(static_cast<const void *>(_textBufferData.data()),
                            _counter * sizeof(uint32_t));
    _transformBuffer->setSubData(static_cast<const void *>(_transformBufferData.data()),
                                 _counter * sizeof(glm::vec4));
}

void TextBuffer::updateBufferSubData(size_t from, size_t to) {
#ifdef _DEBUG
    assert(from > 0);
    assert(from < to);
#endif
    const size_t count = to - from;

    _textBuffer->setSubData(static_cast<const void *>(_textBufferData.data()),
                            count * sizeof(uint32_t), from);
    _transformBuffer->setSubData(static_cast<const void *>(_transformBufferData.data()),
                                 count * sizeof(glm::mat4), from);
}

bool TextBuffer::isFull() const {
    return _counter >= _length;
}

void TextBuffer::bind(uint32_t textBufferBinding, uint32_t transformBufferBinding) const {
    _textBuffer->bind(textBufferBinding);
    _transformBuffer->bind(transformBufferBinding);
}

void TextBuffer::unbind() const {
    _textBuffer->unbind();
    _transformBuffer->unbind();
}

size_t TextBuffer::getLength() const {
    return _length;
}

size_t TextBuffer::getCurrentLength() const {
    return _counter;
}
} // namespace rendell_text
