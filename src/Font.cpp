#include <rendell_text/font.h>

#include <FontLibrary.h>
#include <SharedFontData.h>
#include <logging.h>

#include <hb-ot.h>

#include <cassert>
#include <fstream>
#include <optional>
#include <span>

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
        RT_WARNING("Font file is empty: {}", path.string());
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

// std::shared_ptr<IFont> loadFont(const std::filesystem::path &path) {
//     auto maybeFontData = loadFontFile(path);
//     if (!maybeFontData) {
//         return nullptr;
//     }
//
//     std::vector<std::byte> fontData = std::move(maybeFontData.value());
//     if (fontData.size() == 0) {
//         RT_ERROR("The font file is empty: {}", path.string());
//         return nullptr;
//     }
//
//     return std::make_shared<Font>(std::move(fontData));
// }

static bool create_Msdfgen_stuff(std::span<const std::byte> fontData, MSDF_Resource &result) {
    msdfgen::FreetypeHandle *freetypeHandle = msdfgen::initializeFreetype();
    if (!freetypeHandle) {
        RT_WARNING("FreeType initialization failure using msdfgen");
        return false;
    }

    msdfgen::FontHandle *fontHandle = msdfgen::loadFontData(
        freetypeHandle, reinterpret_cast<const msdfgen::byte *>(fontData.data()),
        static_cast<int>(fontData.size()));
    if (!fontHandle) {
        RT_WARNING("Failed to load msdfgen font");
        msdfgen::deinitializeFreetype(freetypeHandle);
        return false;
    }

    result.fontHandle.reset(fontHandle);
    result.ftHandle.reset(freetypeHandle);
    msdfgen::getFontMetrics(result.metrics, fontHandle);
    return true;
}

bool create_HurfBuzz_stuff(std::span<const std::byte> fontData, HB_Resource &result) {
    hb_blob_t *blob = hb_blob_create(reinterpret_cast<const char *>(fontData.data()),
                                     static_cast<unsigned int>(fontData.size()),
                                     HB_MEMORY_MODE_READONLY, NULL, NULL);
    hb_face_t *face = hb_face_create(blob, 0);
    hb_font_t *font = hb_font_create(face);
    hb_ot_font_set_funcs(font);

    result.blob.reset(blob);
    result.face.reset(face);
    result.font.reset(font);

    return true;
}

static std::unique_ptr<IFontData> createFontData(std::span<const std::byte> rawFontData) {
    std::unique_ptr<SharedFontData> fontData = std::make_unique<SharedFontData>();
    fontData->rawData.assign(rawFontData.begin(), rawFontData.end());
    create_Msdfgen_stuff(fontData->rawData, fontData->msdf);
    create_HurfBuzz_stuff(fontData->rawData, fontData->hb);
    return fontData;
}

FontHandle loadFont(const std::filesystem::path &path) {
    auto maybeFontData = loadFontFile(path);
    if (!maybeFontData) {
        return FontHandle::Empty;
    }

    std::vector<std::byte> rawFontData = std::move(maybeFontData.value());
    if (rawFontData.size() == 0) {
        RT_WARNING("The font file is empty: {}", path.string());
        return FontHandle::Empty;
    }

    std::unique_ptr<IFontData> fontData = createFontData(rawFontData);
    if (!fontData) {
        RT_WARNING("Failed to initialize font: {}", path.string());
        return FontHandle::Empty;
    }

    return FontLibrary::getInstance()->storeFontData(fontData);
}

bool isFontValid(FontHandle fontHandle) {
    return FontLibrary::getInstance()->isFontValid(fontHandle);
}

void releaseFont(FontHandle fontHandle) {
    FontLibrary *fontLibrary = FontLibrary::getInstance();
    if (fontLibrary->isFontValid(fontHandle)) {
        fontLibrary->releaseFontData(fontHandle);
    } else {
        RT_WARNING("Trying to release invalid font handle: {}", static_cast<uint64_t>(fontHandle));
    }
}

} // namespace rendell_text