#include <rendell_text/initialization.h>

#include <TextRenderer.h>
#include <logging.h>

namespace rendell_text {
bool init() {
    if (!TextRenderer::initStaticStuff()) {
        RT_CRITICAL("TextRenderer initialization failure");
        return false;
    }
    return true;
}

void release() {
    TextRenderer::releaseStaticStuff();
}
} // namespace rendell_text