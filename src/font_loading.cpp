#include <rendell_text/font_loading.h>

#include <Font.h>
#include <logging.h>

#include <cassert>
#include <optional>

namespace rendell_text {
static std::optional<std::vector<std::byte>> loadFontFile(const std::filesystem::path &path) {
    assert(!path.empty());

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        RT_ERROR("Failed to open font file: {}", path.string());
        return std::nullopt;
    }

    std::streamsize size = file.tellg();
    if (size <= 0) {
        RT_ERROR("Font file is empty: {}", path.string());
        return std::nullopt;
    }
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char *>(buffer.data()), size)) {
        RT_ERROR("Failed to read font file: {}", path.string());
        return std::nullopt;
    }

    return buffer;
}

std::shared_ptr<IFont> loadFont(const std::filesystem::path &path) {
    auto maybeFontData = loadFontFile(path);
    if (!maybeFontData) {
        return nullptr;
    }

    std::vector<std::byte> fontData = std::move(maybeFontData.value());
    if (fontData.size() == 0) {
        RT_ERROR("The font file is empty: {}", path.string());
        return nullptr;
    }

    return std::make_shared<Font>(std::move(fontData));
}

} // namespace rendell_text