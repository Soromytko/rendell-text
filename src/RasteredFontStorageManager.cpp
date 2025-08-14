#include "RasteredFontStorageManager.h"
#include "FontRaster.h"
#include <algorithm>

namespace rendell_text {
void RasteredFontStorageManager::clearUnusedCache() {
    // This is a lazy cache clearing algorithm.
    std::vector<size_t> releasedKeys;
    releasedKeys.reserve(_rasteredFontStorages.size());
    for (auto it = _rasteredFontStorages.cbegin(); it != _rasteredFontStorages.cend(); it++) {
        if (it->second.use_count() == 1) {
            releasedKeys.push_back(it->first);
        }
    }

    for (const size_t key : releasedKeys) {
        _rasteredFontStorages.erase(key);
    }
}

RasteredFontStorageSharedPtr
RasteredFontStorageManager::getRasteredFontStorage(const RasteredFontStoragePreset &preset) {
    const size_t key = hashFontPreset(preset);
    if (auto it = _rasteredFontStorages.find(key); it != _rasteredFontStorages.end()) {
        return it->second;
    }

    FontRasterSharedPtr fontRaster =
        makeFontRaster(preset.fontPath, preset.fontWidth, preset.fontHeight);
    RasteredFontStorageSharedPtr rasteredFontStorage =
        makeRasteredFontStorage(fontRaster, preset.charRangeSize);
    _rasteredFontStorages[key] = rasteredFontStorage;
    return rasteredFontStorage;
}

size_t RasteredFontStorageManager::hashFontPreset(const RasteredFontStoragePreset &preset) const {
    std::hash<std::string> hasher;
    return hasher(preset.fontPath.string() + std::to_string(preset.fontWidth) +
                  std::to_string(preset.fontHeight));
}
} // namespace rendell_text
