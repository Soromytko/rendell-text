#pragma once
#include <rendell_text/IFont.h>

#include <memory>

namespace rendell_text {
struct FontInstance {
    std::shared_ptr<IFont> font;
    size_t size;
};
} // namespace rendell_text
