#include <rendell_text/Font.h>

#include <cassert>

namespace rendell_text {
Font::Font(std::vector<std::byte> &&data) {
    assert(data.size() > 0);
    _data = std::move(data);
}
} // namespace rendell_text