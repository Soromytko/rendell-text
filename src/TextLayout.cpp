#include <rendell_text/TextLayout.h>

#include <logging.h>

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

const std::unordered_set<TextBatchSharedPtr> &TextLayout::getTextBatchesForRendering() const {
    return _textBatchesForRendering;
}

std::wstring TextLayout::getSubText(size_t indexFrom) const {
    assert(indexFrom < _text.length());
    return std::wstring(_text.begin() + indexFrom, _text.end());
}

void TextLayout::update() {
    updateBuffersIfNeeded();
}

void TextLayout::setGlyphAtlasCache(GlyphAtlasCacheSharedPtr glyphAtlasCache) {
    assert(glyphAtlasCache);
    if (glyphAtlasCache != glyphAtlasCache) {
        _glyphAtlasCache = glyphAtlasCache;
        // TODO: Update
    }
}

void TextLayout::setText(const std::wstring &value) {
    std::wstring text = value;
    setText(std::move(text));
}

void TextLayout::setText(std::wstring &&value) {
    _text = std::move(value);
    _updateActionFlags |= UPDATE_BUFFER_FLAG;
}

GlyphAtlasCacheSharedPtr TextLayout::getGlyphAtlasCache() const {
    return _glyphAtlasCache;
}

const std::wstring &TextLayout::getText() const {
    return _text;
}

size_t TextLayout::getTextLength() const {
    return _text.length();
}

uint32_t TextLayout::getFontHeight() const {
    return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getFontHeight());
}

uint32_t TextLayout::getAscender() const {
    return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getAscender());
}

uint32_t TextLayout::getDescender() const {
    return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getDescender());
}

const std::vector<uint32_t> &TextLayout::getTextAdvance() const {
    updateBuffersIfNeeded();
    return _textAdvance;
}

void TextLayout::eraseText(size_t startIndex) {
    eraseText(startIndex, _text.length() - startIndex);
}

void TextLayout::eraseText(size_t startIndex, size_t count) {
    assert(startIndex >= 0 && startIndex + count <= _text.length());
    _text.erase(startIndex, count);
    _updateActionFlags |= UPDATE_BUFFER_FLAG;
}

void TextLayout::insertText(const std::wstring &text, size_t startIndex) {
    assert(startIndex >= 0 && startIndex <= _text.length());
    _text.insert(startIndex, text);
    _updateActionFlags |= UPDATE_BUFFER_FLAG;
}

void TextLayout::appendText(const std::wstring &text) {
    if (!text.empty()) {
        _text += text;
        _updateActionFlags |= UPDATE_BUFFER_FLAG;
    }
}

static glm::vec2 getInstanceLocalOffset(const RasterizedGlyph &rasterizedChar) {
    const glm::vec2 bearing = rasterizedChar.glyphBearing;
    const glm::vec2 size = rasterizedChar.glyphSize;
    return glm::vec2(bearing.x, bearing.y - size.y);
}

void TextLayout::updateShaderBuffers() const {
    assert(_glyphAtlasCache);

    _textBatchesForRendering.clear();
    _textAdvance.resize(_text.length());
    auto it = _textAdvance.begin();

    glm::vec2 currentOffset(0.0f, 0.0f);
    const size_t length = _text.length();
    for (size_t i = 0; i < length; i++) {
        const Codepoint currentCharacter = _text[i];

        if (currentCharacter == '\n') {
            currentOffset.x = 0.0f;
            currentOffset.y += static_cast<float>(_glyphAtlasCache->getFontHeight());
            continue;
        }

        const GlyphBitmap &glyph = _glyphAtlasCache->getGlyphBitmap(currentCharacter);

        const TextBatchSharedPtr &textBatch = getOrCreateTextBatch(currentCharacter);
        if (!textBatch) {
            RT_ERROR("Failed to create text batch");
            return;
        }

        if (_textBatchesForRendering.find(textBatch) == _textBatchesForRendering.end()) {
            textBatch->beginUpdating();
            _textBatchesForRendering.insert(textBatch);
        }

        const RasterizedGlyph &rasterizedChar =
            textBatch->getRasterizedGlyphRange()->getGlyphByChar(currentCharacter);

        if (currentCharacter != ' ' && currentCharacter != '\t') {
            const glm::vec2 glyphOffset = currentOffset + getInstanceLocalOffset(rasterizedChar);
            textBatch->appendCharacter(currentCharacter, glyphOffset);
        }

        currentOffset.x += (rasterizedChar.glyphAdvance >> 6);
        *it = static_cast<uint32_t>(currentOffset.x);
        it++;
    }

    for (const TextBatchSharedPtr &textBatch : _textBatchesForRendering) {
        textBatch->endUpdating();
    }
}

void TextLayout::updateBuffersIfNeeded() const {
    if (_updateActionFlags & CLEAR_BUFFER_CACHE_FLAG) {
        _rasteredFontStorage = getRasteredFontStorage();
        _cachedTextBatches.clear();
        _textBatchesForRendering.clear();
    }
    if (_updateActionFlags & UPDATE_BUFFER_FLAG) {
        updateShaderBuffers();
    }
    _updateActionFlags = 0;
}

TextBatchSharedPtr TextLayout::getOrCreateTextBatch(wchar_t character) const {
    const wchar_t rangeIndex = _rasteredFontStorage->getRangeIndex(character);
    if (auto it = _cachedTextBatches.find(rangeIndex); it != _cachedTextBatches.end()) {
        return it->second;
    }

    RasterizedGlyphRangeSharedPtr rasterizedGlyphRange =
        _rasteredFontStorage->rasterizeGlyphRange(rangeIndex);
    if (!rasterizedGlyphRange) {
        return nullptr;
    }
    TextBatchSharedPtr result = makeTextBatch(rasterizedGlyphRange, TEXT_BUFFER_SIZE);
    _cachedTextBatches[rangeIndex] = result;
    return result;
}
} // namespace rendell_text
