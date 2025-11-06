#include <rendell_text/initialization.h>

#include <FontLibrary.h>
#include <logging.h>

namespace rendell_text {
bool init() {
    FontLibrary::init(100);
    return true;
}

void release() {
    FontLibrary::release();
}
} // namespace rendell_text