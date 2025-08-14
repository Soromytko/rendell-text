#pragma once
#include <memory>
#include <rendell/rendell.h>
#include <rendell_text/defines.h>
#include <rendell_text/private/IFontRaster.h>

namespace rendell_text {
class GlyphBuffer {
public:
    GlyphBuffer(wchar_t from, wchar_t to, FontRasterizationResult &&fontRasterizationResult);

    void bind(uint32_t textureBlock) const;
    void unbind() const;

    const RasterizedChar &getRasterizedChar(wchar_t character) const;
    const std::vector<RasterizedChar> &getRasterizedChars() const;
    const std::pair<wchar_t, wchar_t> &getRange() const;

private:
    FontRasterizationResult _fontRasterizationResult{};

    std::pair<wchar_t, wchar_t> _range{};
    rendell::Texture2DArraySharedPtr _textures{};
};

RENDELL_TEXT_DECLARE_SHARED_PTR_FACTORY(GlyphBuffer)
} // namespace rendell_text
