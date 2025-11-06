#include <rendell_text/private/TextBatch.h>

#include <cassert>

namespace rendell_text {
TextBatch::TextBatch(RasterizedGlyphRangeSharedPtr rasterizedGlyphRange,
                     rendell::oop::Texture2DArraySharedPtr textures, size_t textBufferSize) {
    assert(rasterizedGlyphRange);
    assert(textures);
    assert(textBufferSize > 0);

    _rasterizedGlyphRange = rasterizedGlyphRange;
    _textures = textures;
    _textBufferSize = textBufferSize;

    _textBuffers.push_back(makeUniqueTextBuffer(textBufferSize));
}

size_t TextBatch::getTextBufferCount() const {
    return _textBuffers.size();
}

void TextBatch::beginUpdating() {
    _counter = 0;
    for (std::unique_ptr<TextBuffer> &textBuffer : _textBuffers) {
        textBuffer->beginUpdating();
    }
}

void TextBatch::appendCharacter(Char character, glm::vec2 offset) {
    assert(_rasterizedGlyphRange);
    assert(_rasterizedGlyphRange->getLength() > 0);
    assert(character >= _rasterizedGlyphRange->at(0).character);

    TextBuffer *textBuffer = _textBuffers[_counter].get();
    if (textBuffer->isFull()) {
        _counter++;
        TextBufferUniquePtr newTextBuffer = makeUniqueTextBuffer(_textBufferSize);
        textBuffer = newTextBuffer.get();
        _textBuffers.push_back(std::move(newTextBuffer));
    }

    const RasterizedGlyph &rasterizedGlyph = _rasterizedGlyphRange->getGlyphByChar(character);
    glm::vec4 glyphTransform(offset, rasterizedGlyph.glyphSize.x, rasterizedGlyph.glyphSize.y);
    textBuffer->appendCharacter(rasterizedGlyph, offset);
}

void TextBatch::endUpdating() {
    _textBuffers.resize(_counter + 1);
    for (std::unique_ptr<TextBuffer> &textBuffer : _textBuffers) {
        textBuffer->endUpdating();
    }
}

void TextBatch::useTexture(rendell::UniformSampler2DId samplerId, uint32_t stage) {
    assert(_textures);
    _textures->use(samplerId, stage);
}

void TextBatch::useTextBuffer(size_t textBufferIndex, uint32_t textBufferBinding,
                              uint32_t transformBufferBinding) {
    assert(textBufferIndex < _textBuffers.size());
    _textBuffers[textBufferIndex]->use(textBufferBinding, transformBufferBinding);
}

const RasterizedGlyphRange *TextBatch::getRasterizedGlyphRange() const {
    return _rasterizedGlyphRange.get();
}

const rendell::oop::Texture2DArray *TextBatch::getTextures() const {
    return _textures.get();
}

const std::vector<TextBufferUniquePtr> &TextBatch::getTextBuffers() const {
    return _textBuffers;
}
} // namespace rendell_text
