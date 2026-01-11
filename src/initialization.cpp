#include <rendell_text/initialization.h>

#include <TextRenderer.h>
#include <logging.h>

namespace rendell_text {
bool init() {
    if (!TextRenderer::initBasicRenderResources()) {
        RT_CRITICAL("TextRenderer initialization failure");
        return false;
    }
    return true;
}

void release() {
    TextRenderer::releaseBasicRenderResources();
}
} // namespace rendell_text