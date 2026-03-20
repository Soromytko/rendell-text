#include <TextLayout.h>

#include <rendell_text/IGlyphAtlasCache.h>
#include <rendell_text/ITextModel.h>

#include <cassert>
#include <memory>

#define CHAR_RANGE_SIZE 200
#define TEXT_BUFFER_SIZE 100

const size_t CLEAR_BUFFER_CACHE_FLAG = 1 << 0;
const size_t UPDATE_BUFFER_FLAG = 1 << 1;

namespace rendell_text {
TextLayout::TextLayout(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache,
                       std::shared_ptr<ITextModel> textModel)
    : _glyphAtlasCache(glyphAtlasCache)
    , _textModel(textModel) {
    assert(_glyphAtlasCache);
    assert(_textModel);
}

bool TextLayout::isEmpty() const {
    assert(_textModel);
    return _textModel->isEmpty();
}

size_t TextLayout::getVersion() const {
    return _version;
}

uint32_t TextLayout::getWidth() const {
}

uint32_t TextLayout::getHeight() const {
    assert(_glyphAtlasCache);
    return static_cast<uint32_t>(_glyphAtlasCache->getLineHeight());
}

std::shared_ptr<IGlyphAtlasCache> TextLayout::getGlyphAtlasCache() const {
    return _glyphAtlasCache;
}

void TextLayout::setGlyphAtlasCache(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) {
    assert(glyphAtlasCache);
    if (glyphAtlasCache != glyphAtlasCache) {
        _glyphAtlasCache = glyphAtlasCache;
        updateBuffers();
    }
}

void TextLayout::supplyText(const rendell_text::String &text) {
    assert(_textModel);
    std::vector<LogicalLine> lines = parseText(text);
    _textModel->setText(std::move(lines));
}

void TextLayout::setAutoSize(bool isAutoWith, bool isAutoHeight) {
    _isAutoWith = isAutoWith;
    _isAutoHeight = isAutoHeight;
}

void TextLayout::eraseText(size_t startIndex) {
   /* assert(startIndex < _text.length());
    eraseText(startIndex, _text.length() - startIndex);*/
}

void TextLayout::eraseText(size_t startIndex, size_t count) {
    //assert(startIndex >= 0 && startIndex + count <= _text.length());
    //_text.erase(startIndex, count);
    //updateBuffers(startIndex);
    //_version++;
}

void TextLayout::insertText(const rendell_text::String &text, size_t startIndex) {
   /* assert(startIndex >= 0 && startIndex <= _text.length());
    _text.insert(startIndex, text);
    updateBuffers(startIndex);
    _version++;*/
}

void TextLayout::appendText(const rendell_text::String &text) {
    //if (!text.empty()) {
    //    const size_t updatedTextLenght = text.length();
    //    _text += text;
    //    updateBuffers(updatedTextLenght);
    //    _version++;
    //}
}

static glm::vec2 getInstanceLocalOffset(const GlyphBitmap &glyphBitmap) {
    const glm::vec2 bearing = glyphBitmap.glyphBearing;
    const glm::vec2 size = {glyphBitmap.width, glyphBitmap.height};
    return glm::vec2(bearing.x, bearing.y - size.y);
}

std::vector<LogicalLine> TextLayout::parseText(const rendell_text::String &text) {
    const auto splitText =
        [](const rendell_text::String &str,
           const rendell_text::String &delimiter) -> std::vector<rendell_text::String> {
        std::vector<rendell_text::String> result;

        rendell_text::String::size_type pos = 0;
        rendell_text::String::size_type prev = 0;
        while ((pos = str.find(delimiter, prev)) != rendell_text::String::npos) {
            result.push_back(str.substr(prev, pos - prev));
            prev = pos + delimiter.length();
        }

        result.push_back(str.substr(prev));

        return result;
    };

    std::vector<rendell_text::String> textLines = splitText(text, U"\n");
    std::vector<LogicalLine> logicalLines;
    logicalLines.resize(textLines.size());
    for (size_t i = 0; i < textLines.size(); i++) {
        VisualLine visualLines = rasterizeString(textLines[i]);
        LogicalLine logicalLine{
            .visualLines = {visualLines},
        };
        logicalLines.push_back(std::move(logicalLine));
    }

    return logicalLines;
}

VisualLine TextLayout::rasterizeString(const String &string) {
    assert(_glyphAtlasCache);

    VisualLine visualLine;
    visualLine.glyphBuffer.reserve(string.size());
    for (size_t i = 0; i < string.size(); i++) {
        const Glyph &glyph = _glyphAtlasCache->getOrRasterizeGlyph(string[i]);
        visualLine.glyphBuffer.push_back({
            .width = glyph.bitmap.width,
            .height = glyph.bitmap.height,
            .bearing = glyph.bitmap.glyphBearing,
            .advance = glyph.bitmap.glyphAdvance,
        });
    }

    return visualLine;
}

void TextLayout::updateBuffers(size_t startFrom) {
  /*  assert(_glyphAtlasCache);

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
            currentOffset.y += static_cast<float>(_glyphAtlasCache->getLineHeight());
            continue;
        }

        const Glyph &glyph = _glyphAtlasCache->getOrRasterizeGlyph(currentCharacter);

        if (currentCharacter != ' ' && currentCharacter != '\t') {
            const glm::vec2 glyphOffset = currentOffset + getInstanceLocalOffset(glyph.bitmap);
            _transforms[i] = glm::vec4(glyphOffset, glyph.bitmap.width, glyph.bitmap.height);
        }

        currentOffset.x += (glyph.bitmap.glyphAdvance >> 6);
    }*/
}

} // namespace rendell_text
