#pragma once
#include <rendell_text/types.h>

#include <cstddef>

namespace rendell_text {
class FontManager {
public:
    FontManager() = default;
    virtual ~FontManager() = default;

    virtual bool isValidFont(FontId fontId) const = 0;

    virtual FontId loadFont(const std::byte *data, size_t size) = 0;
    virtual void releaseFont(FontId fontId) = 0;
};

} // namespace rendell_text

namespace rendell_text {
bool initFontManager();
void releaseFontManager();
FontManager *getFontManager();
} // namespace rendell_text
