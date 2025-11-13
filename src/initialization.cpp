#include <rendell_text/initialization.h>

#include <FontRaster.h>
#include <logging.h>

namespace rendell_text {
bool init() {
    if (!FontRaster::init()) {
        return false;
    }
    return true;
}
} // namespace rendell_text