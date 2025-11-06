#pragma once
#include <rendell_text/types.h>

#include <IFontData.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <vector>

namespace rendell_text {
class FontLibrary final {
public:
    static void init(size_t maxLoadedFontCount);
    static void release();
    static FontLibrary *getInstance();

public:
    class LockGuard final {
    public:
        explicit LockGuard(std::shared_mutex &mutex)
            : _lock(mutex) {}

        LockGuard(LockGuard &&) = default;
        LockGuard &operator=(LockGuard &&) = default;

        LockGuard(const LockGuard &) = delete;
        LockGuard &operator=(const LockGuard &) = delete;

        std::shared_lock<std::shared_mutex> _lock;
    };

private:
    FontLibrary(size_t maxLoadedFontCount);

public:
    FontLibrary(const FontLibrary &) = delete;
    FontLibrary &operator=(const FontLibrary &) = delete;
    FontLibrary(FontLibrary &&) = delete;
    FontLibrary &operator=(FontLibrary &&) = delete;

    ~FontLibrary() = default;

    FontHandle storeFontData(std::unique_ptr<IFontData> &fontData);
    void releaseFontData(FontHandle handle);
    bool isFontValid(FontHandle handle) const;

    [[nodiscard]] LockGuard readAndLock(FontHandle handle, const IFontData *&fontData) const {
        LockGuard lock(_mutex);
        fontData = getFontData(handle);
        return lock;
    };

private:
    struct FontSlot {
        uint16_t version;

        inline bool isValid() const { return data != nullptr; }

        inline void release() { data.reset(); }

        std::unique_ptr<IFontData> data;
    };

    struct FontHandleParts {
        uint16_t index;
        uint16_t version;
    };

    bool isFontSlotValid(uint16_t index, uint16_t version) const;
    IFontData *getFontData(FontHandle handle) const;

    std::optional<size_t> alloc();
    FontHandle createFontHandle(uint16_t index, uint16_t version) const;
    FontHandleParts getFontParts(FontHandle fontHandle) const;

    std::vector<FontSlot> _fontSlots{};

    mutable std::shared_mutex _mutex;
};
} // namespace rendell_text