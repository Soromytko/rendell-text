#include <TextBuffer.h>

#include <rendell_text/ITextLayout.h>

#include <algorithm>
#include <cassert>

#define SHADER_BUFFER_SECTION_SIZE 100

namespace rendell_text {
TextBuffer::TextBuffer(size_t transformUnitSize, size_t uvUnitSize, size_t count = 1024)
    : _count(count) {
    assert(_count > 0);
    _transformShaderBuffer =
        std::make_unique<rendell::oop::ShaderBuffer>(_count * transformUnitSize);
    _uvShaderBuffer = std::make_unique<rendell::oop::ShaderBuffer>(_count * uvUnitSize);

    //_buffers.transformUnitSize = textLayout->getTransformUnitSize();
    //_buffers.uvUnitSize = textLayout->getUVUnitSize();
    // setTextLayout(textLayout);
}

bool TextBuffer::isEmtpy() const {
    return _textLayout->isEmpty();
}

size_t TextBuffer::getLength() const {
    assert(_textLayout);
    return _textLayout->getTextLength();
}

void TextBuffer::setTextLayout(std::shared_ptr<ITextLayout> textLayout) {
    assert(textLayout);
    if (_textLayout != textLayout) {
        _textLayout = textLayout;
        _needsFullUpdate = true;
    }
}

size_t TextBuffer::update(ITextLayout &textLayout, size_t offset) {
    const size_t buffersOffset;
    assert(offset <= _count);
    if (_count == offset) {
        return textLayout.getTextLength();
    }

    auto [transformsBytes, transformSize] = textLayout.getTransforms(buffersOffset);
    auto [uvBytes, uvSize] = textLayout.getUVs(buffersOffset);

    _transformShaderBuffer->setSubData(transformsBytes, transformSize, buffersOffset);
    _uvShaderBuffer->setSubData(uvBytes, uvSize, buffersOffset);
}

void TextBuffer::prepare() {
    assert(_textLayout);

    if (_needsFullUpdate) {
        updateBuffers();
        _textLayoutVersion = _textLayout->getVersion();
        _needsFullUpdate = false;
    } else if (_textLayoutVersion != _textLayout->getVersion()) {
        updateBuffers();
        _textLayoutVersion = _textLayout->getVersion();
    }
}

void TextBuffer::use(uint32_t transformBinding, uint32_t uvBinding) {
    assert(_buffers.transformShaderBuffer);
    assert(_buffers.uvShaderBuffer);
    _buffers.transformShaderBuffer->use(transformBinding);
    _buffers.uvShaderBuffer->use(uvBinding);
}

void TextBuffer::updateBuffers() {
    const size_t textLength =
        std::max(_textLayout->getTextLength() * 2, (size_t)SHADER_BUFFER_SECTION_SIZE);
    _buffers.updateSize(textLength);

    assert(_buffers.transformShaderBuffer);
    assert(_buffers.uvShaderBuffer);
    auto [transformsBytes, transformSize] = _textLayout->getTransforms();
    auto [uvBytes, uvSize] = _textLayout->getUVs();
    _buffers.transformShaderBuffer->setSubData(transformsBytes, transformSize);
    _buffers.uvShaderBuffer->setSubData(uvBytes, uvSize);
}

bool TextBuffer::Buffers::updateSize(size_t textLength) {
    if (_textLength == textLength) {
        return false;
    }
    _textLength = textLength;

    const size_t currentSectionCount = (_textLength / SHADER_BUFFER_SECTION_SIZE) + 1;
    const size_t newSectionCount = (_textLength / SHADER_BUFFER_SECTION_SIZE) + 1;
    const bool shouldResize =
        currentSectionCount < newSectionCount || currentSectionCount - newSectionCount > 1;
    if (shouldResize) {
        const size_t newBufferSize = newSectionCount * SHADER_BUFFER_SECTION_SIZE;
        transformShaderBuffer =
            std::make_unique<rendell::oop::ShaderBuffer>(newBufferSize * transformUnitSize);
        uvShaderBuffer = std::make_unique<rendell::oop::ShaderBuffer>(newBufferSize * uvUnitSize);
        return true;
    }
    return false;
}

} // namespace rendell_text
