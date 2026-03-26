#pragma once
#include <cstdint>
#include <string>

namespace rendell_text {
using Codepoint = char32_t;
using GlyphId = uint16_t;
using String = std::u32string;
using StringView = std::u32string;
} // namespace rendell_text