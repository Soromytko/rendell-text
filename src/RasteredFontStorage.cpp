#include <logging.h>
#include <rendell_text/private/RasteredFontStorage.h>

namespace rendell_text {
RasteredFontStorage::RasteredFontStorage(IFontRasterSharedPtr fontRaster, size_t charRangeSize)
    : _fontRaster(fontRaster)
    , _charRangeSize(charRangeSize) {
    assert(charRangeSize > 0);
}

void RasteredFontStorage::clearCache() {
    _cachedGlyphBuffers.clear();
}

RasterizedGlyphListSharedPtr RasteredFontStorage::rasterizeGlyphRange(size_t rangeIndex) {
    if (auto it = _cachedRasterizedGlyphLists.find(rangeIndex);
        it != _cachedRasterizedGlyphLists.end()) {
        if (auto locked = it->second.lock()) {
            return locked;
        }
    }

    RasterizedGlyphListSharedPtr rasterizedGlyphList = rasterizeRange(rangeIndex);
    _cachedRasterizedGlyphLists[rangeIndex] = rasterizedGlyphList;
    return rasterizedGlyphList;
}

rendell::oop::Texture2DArraySharedPtr
RasteredFontStorage::getOrCreateGlyphTexture(size_t rangeIndex) {
    if (auto it = _cachedGlyphTextures.find(rangeIndex); it != _cachedGlyphTextures.end()) {
        if (auto locked = it->second.lock()) {
            return locked;
        }
    }

    rendell::oop::Texture2DArraySharedPtr glyphBufferPtr = createGlyphTexture(rangeIndex);
    _cachedGlyphTextures[rangeIndex] = glyphBufferPtr;
    return glyphBufferPtr;
}

size_t RasteredFontStorage::getRangeIndex(wchar_t character) const {
    return character / _charRangeSize;
}

uint32_t RasteredFontStorage::getFontWidth() const {
    return _fontWidth;
}

uint32_t RasteredFontStorage::getFontHeight() const {
    return _fontHeight;
}

const IFontRasterSharedPtr RasteredFontStorage::getFontRaster() const {
    return _fontRaster;
}

RasterizedGlyphListSharedPtr RasteredFontStorage::rasterizeRange(size_t rangeIndex) {
    const wchar_t from = static_cast<wchar_t>(rangeIndex * _charRangeSize);
    const wchar_t to = static_cast<wchar_t>(_charRangeSize) + from;
    auto result = _fontRaster->rasterize(from, to);
    if (!result) {
        RT_ERROR("Rasterization failure: {{}, {}}", static_cast<size_t>(from),
                 static_cast<size_t>(to));
        return nullptr;
    }
    return result;
}

rendell::oop::Texture2DArraySharedPtr RasteredFontStorage::createGlyphTexture(size_t rangeIndex) {
    RasterizedGlyphListSharedPtr glyphList = rasterizeGlyphRange(rangeIndex);

    const size_t from = rangeIndex * _charRangeSize;
    const size_t to = _charRangeSize + from;
    const uint32_t charCount = static_cast<uint32_t>(to - from);
    auto texture2DArray = rendell::oop::makeTexture2DArray(
        _fontRaster->getWidth(), _fontRaster->getHeight(), charCount, rendell::TextureFormat::R);

    for (size_t i = 0; i < glyphList->size(); i++) {
        const RasterizedChar &rasteredGlyph = glyphList->operator[](i);
        if (rasteredGlyph.glyphSize.x > 0 && rasteredGlyph.glyphSize.y > 0) {
            texture2DArray->setSubData(
                static_cast<uint32_t>(i), static_cast<uint32_t>(rasteredGlyph.glyphSize.x),
                static_cast<uint32_t>(rasteredGlyph.glyphSize.y),
                static_cast<const rendell::byte_t *>(rasteredGlyph.pixels.data()));
        }
    }

    return texture2DArray;
}
} // namespace rendell_text
