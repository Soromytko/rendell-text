#pragma once
#include <cstdint>
#include <span>
#include <string>

namespace rendell_text {
using Codepoint = char32_t;
using GlyphId = uint16_t;
using String = std::u32string;
using StringView = std::u32string;

enum class FontHandle : uint64_t { Empty = 0 };

struct Size {
    using Type = uint16_t;
    Type width;
    Type height;

    bool operator==(const Size &) const = default;
};

struct FontInstance {
    FontHandle fontHandle;
    Size size;

    bool operator==(const FontInstance &) const = default;
};

using PixelsRef = std::span<const std::byte>;

struct GlyphKey {
    GlyphId id;
    FontInstance fontInstance;

    bool operator==(const GlyphKey &) const = default;
};
} // namespace rendell_text
