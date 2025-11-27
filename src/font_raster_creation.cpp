#include <rendell_text/font_raster_creation.h>

#include "FontRaster.h"

namespace rendell_text {
std::shared_ptr<IFontRaster> createFontRaster() {
    std::shared_ptr<FontRaster> result = std::make_shared<FontRaster>();
    return result;
}
} // namespace rendell_text