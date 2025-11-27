#pragma once
#include "IFontRaster.h"

#include <memory>

namespace rendell_text {
std::shared_ptr<IFontRaster> createFontRaster();
} // namespace rendell_text