#include <rendell_text/FontLoading.h>

#include <logging.h>

#include <cassert>

namespace rendell_text {
std::shared_ptr<Font> loadFont(const std::filesystem::path &path) {
    assert(!path.empty());

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        RT_ERROR("Failed to open font file: {}", path.string());
        return nullptr;
    }

    std::streamsize size = file.tellg();
    if (size <= 0) {
        RT_ERROR("Font file is empty: {}", path.string());
        return nullptr;
    }
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char *>(buffer.data()), size)) {
        RT_ERROR("Failed to read font file: {}", path.string());
        return false;
    }

    return std::make_shared<Font>(std::move(buffer));
}
} // namespace rendell_text