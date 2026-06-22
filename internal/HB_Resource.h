#pragma once
#include <hb.h>
#include <memory>

namespace rendell_text {
struct HB_Resource final {
    template <typename T, auto Method> struct Deleter {
        void operator()(T *ptr) {
            if (ptr) {
                Method(ptr);
            }
        }
    };

    HB_Resource() = default;
    HB_Resource(HB_Resource &&) noexcept = default;
    HB_Resource &operator=(HB_Resource &&) noexcept = default;

    ~HB_Resource() { reset(); }

    using BlobDeleter = Deleter<hb_blob_t, hb_blob_destroy>;
    using FaceDeleter = Deleter<hb_face_t, hb_face_destroy>;
    using FontDeleter = Deleter<hb_font_t, hb_font_destroy>;

    std::unique_ptr<hb_blob_t, BlobDeleter> blob;
    std::unique_ptr<hb_face_t, FaceDeleter> face;
    std::unique_ptr<hb_font_t, FontDeleter> font;

    inline bool isValid() const noexcept { return font && face && blob; }

    inline void reset() {
        font.reset();
        face.reset();
        blob.reset();
    }
};

} // namespace rendell_text
