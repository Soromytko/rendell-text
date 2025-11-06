#pragma once
#include <IFontData.h>

#include <HB_Resource.h>
#include <MSDF_Resource.h>

namespace rendell_text {
struct SharedFontData final : IFontData {
    HB_Resource hb;
    MSDF_Resource msdf;
};
} // namespace rendell_text