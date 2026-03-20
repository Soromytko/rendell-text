#pragma once
#include "types.h"

#include <memory>
#include <utility>

namespace rendell_text {
class IGlyphAtlasCache;
} // namespace rendell_text

namespace rendell_text {
class ITextLayout {
public:
    ITextLayout() = default;
    virtual ~ITextLayout() = default;

    virtual bool isEmpty() const = 0;

    virtual size_t getVersion() const = 0;
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual std::shared_ptr<IGlyphAtlasCache> getGlyphAtlasCache() const = 0;

    virtual void setGlyphAtlasCache(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) = 0;
    virtual void supplyText(const rendell_text::String &text) = 0;
    virtual void setAutoSize(bool isAutoWith, bool isAutoHeight) = 0;

    virtual void eraseText(size_t startIndex) = 0;
    virtual void eraseText(size_t startIndex, size_t count) = 0;
    virtual void insertText(const rendell_text::String &text, size_t startIndex = 0) = 0;
    virtual void appendText(const rendell_text::String &text) = 0;
};

} // namespace rendell_text