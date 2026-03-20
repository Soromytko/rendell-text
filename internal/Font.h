#pragma once
#include <rendell_text/IFont.h>

#include <vector>

namespace rendell_text {
class Font final : public IFont {
public:
    Font(std::vector<std::byte> data);
    ~Font() = default;

    size_t getSize() const override;
    const std::byte *getData() const override;

private:
    std::vector<std::byte> _data;
};
} // namespace rendell_text