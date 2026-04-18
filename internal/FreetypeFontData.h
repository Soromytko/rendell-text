#pragma once
#include <IFontData.h>

#include <cstddef>
#include <memory>
#include <vector>

namespace rendell_text {
class FreetypeFontData final : public IFontData {};

std::unique_ptr<IFontData> createFreetypeFontData(const std::vector<std::byte> &data);
} // namespace rendell_text