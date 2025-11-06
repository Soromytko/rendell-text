#include <rendell_text/private/GlyphBuffer.h>

namespace rendell_text {
GlyphBuffer::GlyphBuffer(wchar_t from, wchar_t to, RasterizedGlyphListSharedPtr rasterizedGlyphList)
    : _rasterizedGlyphList(rasterizedGlyphList) {
#ifdef _DEBUG
    assert(from >= 0);
    assert(from < to);
#endif
    _range = {from, to};

    _textures = _fontRasterizationResult.texture2DArray;
}

void GlyphBuffer::use(rendell::UniformSampler2DId uniformSampler2DId, uint32_t textureBlock) const {
    _textures->use(uniformSampler2DId, textureBlock);
}

const RasterizedGlyphListSharedPtr &GlyphBuffer::getRasterizedChars() const {
    return _rasterizedGlyphList;
}

const std::pair<wchar_t, wchar_t> &GlyphBuffer::getRange() const {
    return _range;
}
} // namespace rendell_text
