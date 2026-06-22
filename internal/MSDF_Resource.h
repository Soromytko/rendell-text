#pragma once
#include <msdfgen-ext.h>

#include <memory>

namespace rendell_text {
struct MSDF_Resource final {
    struct FontDeleter {
        void operator()(msdfgen::FontHandle *ptr) {
            if (ptr) {
                msdfgen::destroyFont(ptr);
            }
        }
    };

    struct FtDeleter {
        void operator()(msdfgen::FreetypeHandle *ptr) {
            if (ptr) {
                msdfgen::deinitializeFreetype(ptr);
            }
        }
    };

    MSDF_Resource() = default;
    MSDF_Resource(MSDF_Resource &&) noexcept = default;
    MSDF_Resource &operator=(MSDF_Resource &&) noexcept = default;

    ~MSDF_Resource() { reset(); }

    std::unique_ptr<msdfgen::FontHandle, FontDeleter> fontHandle;
    std::unique_ptr<msdfgen::FreetypeHandle, FtDeleter> ftHandle;
    msdfgen::FontMetrics metrics;

    inline bool isValid() const noexcept { return fontHandle && ftHandle; }

    inline void reset() noexcept {
        fontHandle.reset();
        ftHandle.reset();
    }
};

} // namespace rendell_text
