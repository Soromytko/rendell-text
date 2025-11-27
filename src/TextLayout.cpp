#include <TextLayout.h>
#include <TextLayout.h>

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#define CHAR_RANGE_SIZE 200
#define TEXT_BUFFER_SIZE 100

const size_t CLEAR_BUFFER_CACHE_FLAG = 1 << 0;
const size_t UPDATE_BUFFER_FLAG = 1 << 1;

namespace rendell_text {
TextLayout::TextLayout(std::shared_ptr<GlyphAtlasCache> glyphAtlasCache) {
    assert(_glyphAtlasCache);
    _glyphAtlasCache = glyphAtlasCache;
}

bool TextLayout::isEmpty() const {
    return _text.length() == 0;
}

std::shared_ptr<GlyphAtlasCache> TextLayout::getGlyphAtlasCache() const {
    return _glyphAtlasCache;
}

const std::u32string &TextLayout::getText() const {
    return _text;
}

size_t TextLayout::getTextLength() const {
    return _text.length();
}

uint32_t TextLayout::getHeight() const {
    return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getFontHeight());
}

uint32_t TextLayout::getAscender() const {
    return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getAscender());
}

uint32_t TextLayout::getDescender() const {
    return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getDescender());
}

const std::vector<uint32_t> &TextLayout::getTextAdvance() const {
    return _textAdvance;
}

std::u32string TextLayout::getSubText(size_t indexFrom) const {
    assert(indexFrom < _text.length());
    return std::u32string(_text.begin() + indexFrom, _text.end());
}

void TextLayout::update() {
    if (_isDirty) {
        updateBuffers();
        _isDirty = false;
    }
}

void TextLayout::setGlyphAtlasCache(std::shared_ptr<GlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    if (glyphAtlasCache != glyphAtlasCache) {
        _glyphAtlasCache = glyphAtlasCache;
        updateBuffers();
    }
}

void TextLayout::setText(const std::u32string &value) {
    std::u32string text = value;
    setText(std::move(text));
}

void TextLayout::setText(std::u32string &&value) {
    if (_text != value) {
        _text = std::move(value);
        _isDirty = true;
    }
}

void TextLayout::eraseText(size_t startIndex) {
    assert(startIndex < _text.length());
    eraseText(startIndex, _text.length() - startIndex);
    _isDirty = true;
}

void TextLayout::eraseText(size_t startIndex, size_t count) {
    assert(startIndex >= 0 && startIndex + count <= _text.length());
    _text.erase(startIndex, count);
    updateBuffers(startIndex);
    _isDirty = true;
}

void TextLayout::insertText(const std::u32string &text, size_t startIndex) {
    assert(startIndex >= 0 && startIndex <= _text.length());
    _text.insert(startIndex, text);
    updateBuffers(startIndex);
    _isDirty = true;
}

void TextLayout::appendText(const std::u32string &text) {
    if (!text.empty()) {
        const size_t updatedTextLenght = text.length();
        _text += text;
        updateBuffers(updatedTextLenght);
        _isDirty = true;
    }
}

static glm::vec2 getInstanceLocalOffset(const GlyphBitmap &glyphBitmap) {
    const glm::vec2 bearing = glyphBitmap.glyphBearing;
    const glm::vec2 size = glyphBitmap.glyphSize;
    return glm::vec2(bearing.x, bearing.y - size.y);
}

void TextLayout::updateBuffers(size_t startFrom) {
    assert(_glyphAtlasCache);

    const size_t textLength = _text.length();
    assert(startFrom < textLength);

    _uvs.resize(textLength);
    _transforms.resize(textLength);
    _textAdvance.resize(textLength);

    glm::vec2 currentOffset(0.0f, 0.0f);
    for (size_t i = startFrom; i < textLength; i++) {
        const Codepoint currentCharacter = _text[i];

        if (currentCharacter == '\n') {
            currentOffset.x = 0.0f;
            currentOffset.y += static_cast<float>(_glyphAtlasCache->getFontHeight());
            continue;
        }

        const Glyph &glyph = _glyphAtlasCache->getOrRasterizeGlyph(currentCharacter);

        if (currentCharacter != ' ' && currentCharacter != '\t') {
            const glm::vec2 glyphOffset = currentOffset + getInstanceLocalOffset(glyph.bitmap);
            _transforms[i] =
                glm::vec4(glyphOffset, glyph.bitmap.glyphSize.x, glyph.bitmap.glyphSize.y);
        }

        currentOffset.x += (glyph.bitmap.glyphAdvance >> 6);
    }
}

} // namespace rendell_text
