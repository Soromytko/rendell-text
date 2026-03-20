#pragma once
#include <cstddef>

namespace rendell_text {
class IFont {
public:
    IFont() = default;
    virtual ~IFont() = default;

    virtual size_t getSize() const = 0;
    virtual const std::byte *getData() const = 0;
};
} // namespace rendell_text
