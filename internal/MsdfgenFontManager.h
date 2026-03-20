#pragma once
#include <FontManager.h>

#include <memory>
#include <unordered_map>

namespace msdfgen {
class FreetypeHandle;
class FontHandle;
} // namespace msdfgen

namespace rendell_text {
class MsdfgenFontManager final : public FontManager {
public:
    MsdfgenFontManager();
    ~MsdfgenFontManager();

    bool isValidFont(FontId fontId) const override;

    FontId loadFont(const std::byte *data, size_t size) override;
    void releaseFont(FontId fontId) override;

private:
    struct FontDeleter {
        void operator()(msdfgen::FontHandle *ptr) {
            if (ptr) {
                msdfgen::destroyFont(ptr);
            }
        }
    };

    struct FontData {
        FontData(msdfgen::FontHandle *handle)
            : handle(handle) {}

        FontData() = delete;
        FontData(const FontData &other) = delete;
        FontData(FontData &&other) = default;

        std::unique_ptr<msdfgen::FontHandle, FontDeleter> handle{nullptr};
    };

    msdfgen::FreetypeHandle *_freetypeHandle{nullptr};

    std::unordered_map<FontId, FontData> _fonts;
};
} // namespace rendell_text
