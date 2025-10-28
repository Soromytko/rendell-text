#include "RasteredFontStorageManager.h"

#include <rendell_text/TextLayout.h>
#include <rendell_text/private/IFontRaster.h>

#include <logging.h>

#include <cassert>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <utility>

#define CHAR_RANGE_SIZE 200
#define TEXT_BUFFER_SIZE 100

static constexpr glm::ivec2 DEFAULT_FONT_SIZE = glm::ivec2(64, 64);

const size_t CLEAR_BUFFER_CACHE_FLAG = 1 << 0;
const size_t UPDATE_BUFFER_FLAG = 1 << 1;

namespace rendell_text {
static std::unique_ptr<RasteredFontStorageManager> s_rasteredFontStorageManager;
static uint32_t s_instanceCount{};
static bool s_initialized = false;

static bool initStaticRendererStuff() {
    s_rasteredFontStorageManager.reset(new RasteredFontStorageManager);
    return true;
}

static void releaseStaticRendererStuff() {
    s_rasteredFontStorageManager.reset(nullptr);
    s_initialized = false;
}

TextLayout::TextLayout()
    : _fontSize(DEFAULT_FONT_SIZE) {
    s_instanceCount++;
    init();
}

TextLayout::TextLayout(TextLayout &&other) noexcept {
    _fontSize = std::exchange(other._fontSize, DEFAULT_FONT_SIZE);
    _fontPath = std::exchange(other._fontPath, "");
    _text = std::move(other._text);
    _isUpdating = other._isUpdating;
    _rasteredFontStorage = std::exchange(other._rasteredFontStorage, nullptr);
    _cachedTextBatches = std::move(other._cachedTextBatches);
    _textBatchesForRendering = std::move(other._textBatchesForRendering);
    _textAdvance = std::move(other._textAdvance);
    _updateActionFlags = other._updateActionFlags;
}

TextLayout::~TextLayout() {
    // Release it to check the cache.
    _rasteredFontStorage.reset();
    s_rasteredFontStorageManager->clearUnusedCache();

    s_instanceCount--;
    if (s_instanceCount == 0) {
        releaseStaticRendererStuff();
    }
}

bool TextLayout::isInitialized() const {
    return s_initialized;
}

const std::unordered_set<TextBatchSharedPtr> &TextLayout::getTextBatchesForRendering() const {
    return _textBatchesForRendering;
}

std::wstring TextLayout::getSubText(size_t indexFrom) const {
    assert(indexFrom < _text.length());
    return std::wstring(_text.begin() + indexFrom, _text.end());
}

void TextLayout::beginUpdating() {
    assert(!_isUpdating);
    _isUpdating = true;
}

void TextLayout::setFontPath(const std::filesystem::path &fontPath) {
    assert(_isUpdating);
    if (_fontPath != fontPath) {
        _fontPath = fontPath;
        _rasteredFontStorage = getRasteredFontStorage();
        _updateActionFlags |= CLEAR_BUFFER_CACHE_FLAG;
        _updateActionFlags |= UPDATE_BUFFER_FLAG;
    }
}

void TextLayout::setText(const std::wstring &value) {
    assert(_isUpdating);
    std::wstring text = value;
    setText(std::move(text));
}

void TextLayout::setText(std::wstring &&value) {
    assert(_isUpdating);
    _text = std::move(value);
    _updateActionFlags |= UPDATE_BUFFER_FLAG;
}

void TextLayout::setFontSize(const glm::ivec2 &fontSize) {
    assert(_isUpdating);
    if (_fontSize != fontSize) {
        _fontSize = fontSize;
        _rasteredFontStorage = getRasteredFontStorage();
        _updateActionFlags |= CLEAR_BUFFER_CACHE_FLAG;
        _updateActionFlags |= UPDATE_BUFFER_FLAG;
    }
}

void TextLayout::endUpdating() {
    assert(_isUpdating);
    updateBuffersIfNeeded();
    _isUpdating = false;
}

bool TextLayout::isEmpty() const {
    return _text.size() == 0;
}

const std::filesystem::path &TextLayout::getFontPath() const {
    if (_rasteredFontStorage) {
        return _rasteredFontStorage->getFontRaster()->getFontPath();
    }

    static const std::filesystem::path emptyPath;
    return emptyPath;
}

glm::ivec2 TextLayout::getFontSize() const {
    return _fontSize;
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
    assert(!_isUpdating);
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

bool TextLayout::init() {
    if (!s_initialized) {
        s_initialized = initStaticRendererStuff();
    }

    if (!s_initialized) {
        return false;
    }

    return s_initialized;
}

static glm::vec2 getInstanceLocalOffset(const RasterizedChar &rasterizedChar) {
    const glm::vec2 bearing = rasterizedChar.glyphBearing;
    const glm::vec2 size = rasterizedChar.glyphSize;
    return glm::vec2(bearing.x, bearing.y - size.y);
}

void TextLayout::updateShaderBuffers() {
    _textBatchesForRendering.clear();
    _textAdvance.resize(_text.length());
    auto it = _textAdvance.begin();

    glm::vec2 currentOffset(0.0f, 0.0f);
    const size_t length = _text.length();
    for (size_t i = 0; i < length; i++) {
        const wchar_t currentCharacter = _text[i];

        if (currentCharacter == '\n') {
            currentOffset.x = 0.0f;
            currentOffset.y += _fontSize.y;
            continue;
        }

        const TextBatchSharedPtr &textBatch = createTextBatch(currentCharacter);
        if (!textBatch) {
            std::cout << "ERROR::TextLayout: Failed to create text batch";
            return;
        }

        if (_textBatchesForRendering.find(textBatch) == _textBatchesForRendering.end()) {
            textBatch->beginUpdating();
            _textBatchesForRendering.insert(textBatch);
        }

        const RasterizedChar &rasterizedChar =
            textBatch->getGlyphBuffer()->getRasterizedChar(currentCharacter);

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

void TextLayout::updateBuffersIfNeeded() {
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

RasteredFontStorageSharedPtr TextLayout::getRasteredFontStorage() const {
    RasteredFontStoragePreset preset{
        _fontPath.string(),
        static_cast<uint32_t>(_fontSize.x),
        static_cast<uint32_t>(_fontSize.y),
        CHAR_RANGE_SIZE,
    };
    const RasteredFontStorageSharedPtr result =
        s_rasteredFontStorageManager->getRasteredFontStorage(preset);
    s_rasteredFontStorageManager->clearUnusedCache();
    return result;
}

TextBatchSharedPtr TextLayout::createTextBatch(wchar_t character) {
    const wchar_t rangeIndex = _rasteredFontStorage->getRangeIndex(character);
    if (auto it = _cachedTextBatches.find(rangeIndex); it != _cachedTextBatches.end()) {
        return it->second;
    }

    GlyphBufferSharedPtr glyphBuffer = _rasteredFontStorage->rasterizeGlyphRange(rangeIndex);
    if (!glyphBuffer) {
        return nullptr;
    }
    TextBatchSharedPtr result = makeTextBatch(glyphBuffer, TEXT_BUFFER_SIZE);
    _cachedTextBatches[static_cast<uint32_t>(rangeIndex)] = result;
    return result;
}
} // namespace rendell_text
