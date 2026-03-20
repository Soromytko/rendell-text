#include <MsdfgenFontManager.h>

#include <logging.h>

#include <cassert>

#include <msdfgen-ext.h>

namespace rendell_text {
MsdfgenFontManager::MsdfgenFontManager() {
    _freetypeHandle = msdfgen::initializeFreetype();
    if (!_freetypeHandle) {
        RT_CRITICAL("FreeType initialization failure using msdfgen!");
    }
}

MsdfgenFontManager::~MsdfgenFontManager() {
    _fonts.clear();
    if (_freetypeHandle) {
        msdfgen::deinitializeFreetype(_freetypeHandle);
    }
}

bool MsdfgenFontManager::isValidFont(FontId fontId) const {
    const auto it = _fonts.find(fontId);
    return it != _fonts.end();
}

static FontId generateFontId(auto *handle) {
    return static_cast<FontId>(reinterpret_cast<uintptr_t>(handle));
}

FontId MsdfgenFontManager::loadFont(const std::byte *data, size_t size) {
    assert(data);
    assert(size > 0);
    assert(_freetypeHandle);

    const auto handle = msdfgen::loadFontData(
        _freetypeHandle, reinterpret_cast<const msdfgen::byte *>(data), static_cast<int>(size));
    if (!handle) {
        RT_ERROR("Failed to load msdfgen font!");
        return FontId::Invalid;
    }

    FontData fontData(handle);
    const FontId fontId = generateFontId(handle);
    _fonts.insert({fontId, std::move(fontData)});
    return fontId;
}

void MsdfgenFontManager::releaseFont(FontId fontId) {
    const auto it = _fonts.find(fontId);
    assert(it != _fonts.end());
    _fonts.erase(it);
}

} // namespace rendell_text