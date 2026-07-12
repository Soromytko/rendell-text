#pragma once
#include <array>
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

    [[nodiscard]]
    constexpr size_t area() const noexcept {
        return static_cast<size_t>(width) * static_cast<size_t>(height);
    }
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

enum class AtlasType {
    bitmap,
    sdf,
    msdf,
    mtsdf,
};

constexpr size_t bytesPerPixel(AtlasType type) {
    switch (type) {
    case AtlasType::bitmap: {
        // RGBA
        return 4;
    }
    case AtlasType::sdf: {
        // R
        return 1;
    }
    case AtlasType::msdf: {
        // RGBA
        return 4;
    }
    case AtlasType::mtsdf: {
        // RGBA
        return 4;
    }
    }
    return 0;
}
} // namespace rendell_text
