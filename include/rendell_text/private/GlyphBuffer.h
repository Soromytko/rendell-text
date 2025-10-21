#pragma once
#include <memory>
#include <rendell/oop/rendell_oop.h>
#include <rendell/rendell.h>
#include <rendell_text/defines.h>
#include <rendell_text/private/IFontRaster.h>

namespace rendell_text {
class GlyphBuffer {
public:
    GlyphBuffer(wchar_t from, wchar_t to, FontRasterizationResult &&fontRasterizationResult);

    void use(rendell::UniformSampler2DId uniformSampler2DId, uint32_t textureBlock) const;

    const RasterizedChar &getRasterizedChar(wchar_t character) const;
    const std::vector<RasterizedChar> &getRasterizedChars() const;
    const std::pair<wchar_t, wchar_t> &getRange() const;

private:
    FontRasterizationResult _fontRasterizationResult{};

    std::pair<wchar_t, wchar_t> _range{};
    rendell::oop::Texture2DArraySharedPtr _textures{};
};

RENDELL_TEXT_DECLARE_SHARED_PTR_FACTORY(GlyphBuffer)
} // namespace rendell_text
