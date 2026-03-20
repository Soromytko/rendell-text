#include <FontManager.h>

#include <MsdfgenFontManager.h>

#include <cassert>
#include <memory>

namespace rendell_text {
static std::unique_ptr<FontManager> s_fontManager{};

bool initFontManager() {
    assert(!s_fontManager);
    s_fontManager = std::make_unique<MsdfgenFontManager>();
}

void releaseFontManager() {
    assert(s_fontManager);
    s_fontManager.reset();
}

FontManager *getFontManager() {
    assert(s_fontManager);
    return s_fontManager.get();
}

} // namespace rendell_text
