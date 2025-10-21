#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <rendell_text/private/TextBatch.h>

namespace rendell_text {
TextBuffer::TextBuffer(size_t length)
    : _length(length) {
    _textBufferData.resize(_length);
    _transformBufferData.resize(_length);

    // TODO: Right now rendell requires that the data is not null.
    const std::vector<rendell::byte_t> textEmptyData(_length * sizeof(uint32_t));
    const std::vector<rendell::byte_t> transformEmptyData(_length * sizeof(glm::mat4));
    _textBuffer = rendell::oop::makeShaderBuffer(textEmptyData.data(), _length * sizeof(uint32_t));
    _transformBuffer =
        rendell::oop::makeShaderBuffer(transformEmptyData.data(), _length * sizeof(glm::mat4));
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
    if (_counter == 0) {
        return;
    }
    _textBuffer->setSubData(reinterpret_cast<const rendell::byte_t *>(_textBufferData.data()),
                            _counter * sizeof(uint32_t));
    _transformBuffer->setSubData(
        reinterpret_cast<const rendell::byte_t *>(_transformBufferData.data()),
        _counter * sizeof(glm::vec4));
}

void TextBuffer::updateBufferSubData(size_t from, size_t to) {
#ifdef _DEBUG
    assert(from > 0);
    assert(from < to);
#endif
    const size_t count = to - from;

    _textBuffer->setSubData(reinterpret_cast<const rendell::byte_t *>(_textBufferData.data()),
                            count * sizeof(uint32_t), from);
    _transformBuffer->setSubData(
        reinterpret_cast<const rendell::byte_t *>(_transformBufferData.data()),
        count * sizeof(glm::mat4), from);
}

bool TextBuffer::isFull() const {
    return _counter >= _length;
}

void TextBuffer::use(uint32_t textBufferBinding, uint32_t transformBufferBinding) const {
    _textBuffer->use(textBufferBinding);
    _transformBuffer->use(transformBufferBinding);
}

size_t TextBuffer::getLength() const {
    return _length;
}

size_t TextBuffer::getCurrentLength() const {
    return _counter;
}
} // namespace rendell_text
