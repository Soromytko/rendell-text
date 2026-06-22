#pragma once
#include <IFontData.h>

#include <HB_Resource.h>
#include <MSDF_Resource.h>

namespace rendell_text {
struct SharedFontData final : IFontData {
    std::vector<std::byte> rawData;
    HB_Resource hb;
    MSDF_Resource msdf;

    ~SharedFontData() {
        hb.reset();
        msdf.reset();
        rawData.clear();
    }
};
} // namespace rendell_text