#pragma once
#include <rendell/DataType.h>

#include <memory>
#include <string>
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
    virtual std::shared_ptr<IGlyphAtlasCache> getGlyphAtlasCache() const = 0;
    virtual const std::u32string &getText() const = 0;
    virtual size_t getTextLength() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual uint32_t getAscender() const = 0;
    virtual uint32_t getDescender() const = 0;
    virtual const std::vector<uint32_t> &getTextAdvance() const = 0;
    virtual size_t getTransformUnitSize() const = 0;
    virtual size_t getUVUnitSize() const = 0;
    virtual std::pair<const rendell::byte_t *, size_t> getTransforms() const = 0;
    virtual std::pair<const rendell::byte_t *, size_t> getUVs() const = 0;

    virtual std::u32string getSubText(size_t indexFrom) const = 0;

    virtual void setGlyphAtlasCache(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) = 0;
    virtual void setText(const std::u32string &value) = 0;
    virtual void setText(std::u32string &&value) = 0;

    virtual void eraseText(size_t startIndex) = 0;
    virtual void eraseText(size_t startIndex, size_t count) = 0;
    virtual void insertText(const std::u32string &text, size_t startIndex = 0) = 0;
    virtual void appendText(const std::u32string &text) = 0;
};

} // namespace rendell_text