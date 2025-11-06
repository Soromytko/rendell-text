#pragma once
#include <rendell/oop/raii.h>
#include <rendell/oop/rendell_oop.h>
#include <rendell/rendell.h>
#include <rendell_text/private/IFontRaster.h>

#include <memory>

namespace rendell_text {
class GlyphBuffer {
public:
    GlyphBuffer(wchar_t from, wchar_t to, RasterizedGlyphListSharedPtr rasterizedGlyphList);

    void use(rendell::UniformSampler2DId uniformSampler2DId, uint32_t textureBlock) const;

    const RasterizedGlyphListSharedPtr &getRasterizedGlyphList() const;
    const std::pair<wchar_t, wchar_t> &getRange() const;

private:
    RasterizedGlyphListSharedPtr _rasterizedGlyphList{};
    std::pair<wchar_t, wchar_t> _range{};
    rendell::oop::Texture2DArraySharedPtr _textures{};
};

RENDELL_USE_RAII_FACTORY(GlyphBuffer)
} // namespace rendell_text
