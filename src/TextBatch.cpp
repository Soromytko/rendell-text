#include <rendell_text/private/TextBatch.h>

namespace rendell_text {
TextBatch::TextBatch(GlyphBufferSharedPtr glyphBuffer, size_t textBufferSize) {
    _glyphBuffer = glyphBuffer;
    _textBufferSize = textBufferSize;

    _textBuffers.push_back(std::make_unique<TextBuffer>(textBufferSize));
}

void TextBatch::beginUpdating() {
    _counter = 0;
    for (std::unique_ptr<TextBuffer> &textBuffer : _textBuffers) {
        textBuffer->beginUpdating();
    }
}

void TextBatch::appendCharacter(wchar_t character, glm::vec2 offset) {
    TextBuffer *textBuffer = _textBuffers[_counter].get();

    if (textBuffer->isFull()) {
        _counter++;
        textBuffer = new TextBuffer(_textBufferSize);
        _textBuffers.push_back(std::unique_ptr<TextBuffer>(textBuffer));
    }

    const RasterizedChar &rasterizedChar = _glyphBuffer->getRasterizedChar(character);
    glm::vec4 glyphTransform(offset, rasterizedChar.glyphSize.x, rasterizedChar.glyphSize.y);
    textBuffer->appendCharacter(rasterizedChar, offset);
}

void TextBatch::endUpdating() {
    _textBuffers.resize(_counter + 1);
    for (std::unique_ptr<TextBuffer> &textBuffer : _textBuffers) {
        textBuffer->endUpdating();
    }
}

const GlyphBuffer *TextBatch::getGlyphBuffer() const {
    return _glyphBuffer.get();
}

const std::vector<TextBufferUniquePtr> &TextBatch::getTextBuffers() const {
    return _textBuffers;
}
} // namespace rendell_text
