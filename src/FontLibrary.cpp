#include <FontLibrary.h>

#include <IFontData.h>
#include <logging.h>

#include <cassert>
#include <memory>

namespace rendell_text {
static std::unique_ptr<FontLibrary> s_fontManager{};

void FontLibrary::init(size_t maxLoadedFontCount) {
    assert(!s_fontManager);
    s_fontManager = std::unique_ptr<FontLibrary>(new FontLibrary(maxLoadedFontCount));
}

void FontLibrary::release() {
    assert(s_fontManager);
    s_fontManager.reset();
}

FontLibrary *FontLibrary::getInstance() {
    assert(s_fontManager);
    return s_fontManager.get();
}

FontLibrary::FontLibrary(size_t maxLoadedFontCount) {
    assert(maxLoadedFontCount > 0);
    _fontSlots.resize(maxLoadedFontCount);
}

FontHandle FontLibrary::storeFontData(std::unique_ptr<IFontData> &fontData) {
    assert(fontData);

    std::unique_lock<std::shared_mutex> lock(_mutex);

    const auto maybeSlotIndex = alloc();
    if (!maybeSlotIndex) {
        RT_WARNING("Maximum loaded fonts reached ({})", _fontSlots.size());
        return FontHandle::Empty;
    }

    const auto slotIndex = maybeSlotIndex.value();
    assert(slotIndex < _fontSlots.size());
    FontSlot &fontSlot = _fontSlots[slotIndex];
    fontSlot.data = std::move(fontData);

    return createFontHandle(static_cast<uint16_t>(slotIndex), fontSlot.version);
}

void FontLibrary::releaseFontData(FontHandle handle) {
    std::unique_lock<std::shared_mutex> lock(_mutex);

    FontHandleParts parts = getFontParts(handle);
    if (!isFontSlotValid(parts.index, parts.version)) {
        RT_WARNING("Invalid FontHandle: {}", static_cast<uint64_t>(handle));
        return;
    }
    _fontSlots[parts.index].release();
}

bool FontLibrary::isFontValid(FontHandle handle) const {
    std::shared_lock<std::shared_mutex> lock(_mutex);

    FontHandleParts parts = getFontParts(handle);
    return isFontSlotValid(parts.index, parts.version);
}

bool FontLibrary::isFontSlotValid(uint16_t index, uint16_t version) const {
    return index < _fontSlots.size() && _fontSlots[index].isValid();
}

IFontData *FontLibrary::getFontData(FontHandle handle) const {
    const auto parts = getFontParts(handle);
    if (isFontSlotValid(parts.index, parts.index)) {
        assert(parts.index < _fontSlots.size());
        return _fontSlots[parts.index].data.get();
    }
    return nullptr;
}

std::optional<size_t> FontLibrary::alloc() {
    for (size_t i = 0; i < _fontSlots.size(); i++) {
        FontSlot &fontSlots = _fontSlots[i];
        if (!fontSlots.isValid()) {
            fontSlots.version++;
            return i;
        }
    }
    return std::nullopt;
}

FontHandle FontLibrary::createFontHandle(uint16_t index, uint16_t version) const {
    const uint64_t value = (static_cast<uint64_t>(version) << 16) | index;
    return static_cast<FontHandle>(value);
}

FontLibrary::FontHandleParts FontLibrary::getFontParts(FontHandle fontHandle) const {
    const uint64_t value = static_cast<uint64_t>(fontHandle);
    return {
        .index = static_cast<uint16_t>(value & 0xFFFF),
        .version = static_cast<uint16_t>(value >> 16),
    };
}

} // namespace rendell_text
