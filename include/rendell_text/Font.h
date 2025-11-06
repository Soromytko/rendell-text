#pragma once
#include <rendell/oop/raii.h>

#include <string>
#include <vector>

namespace rendell_text {
class Font final {
public:
    Font(std::vector<std::byte> &&data);

    inline bool isEmpty() const { return _data.size(); }

    inline const std::vector<std::byte> &getData() const { return _data; }

private:
    std::vector<std::byte> _data;
};
RENDELL_USE_RAII(Font)
} // namespace rendell_text