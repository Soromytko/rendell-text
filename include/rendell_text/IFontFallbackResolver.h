#pragma once

namespace rendell_text {
class IFontFallbackResolver {
public:
    IFontFallbackResolver() = default;
    virtual ~IFontFallbackResolver() = default;
};
} // namespace rendell_text