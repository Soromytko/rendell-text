#include <Font.h>

#include <cassert>

namespace rendell_text {
Font::Font(std::vector<std::byte> data)
    : _data(std::move(data)) {
    assert(_data.size() > 0);
}

size_t Font::getSize() const {
    return _data.size();
}

const std::byte *Font::getData() const {
    return _data.data();
}
} // namespace rendell_text