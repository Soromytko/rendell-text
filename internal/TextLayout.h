#pragma once
#include <rendell_text/ITextLayout.h>

#include <glm/glm.hpp>
#include <vector>

namespace rendell_text {
class TextLayout final : public ITextLayout {
public:
    TextLayout(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache);
    ~TextLayout() = default;

    bool isEmpty() const override;

    std::shared_ptr<IGlyphAtlasCache> getGlyphAtlasCache() const override;
    const std::u32string &getText() const override;
    size_t getTextLength() const override;
    uint32_t getHeight() const override;
    uint32_t getAscender() const override;
    uint32_t getDescender() const override;
    const std::vector<uint32_t> &getTextAdvance() const override;

    std::u32string getSubText(size_t indexFrom) const override;

    void setChangedCallback(ChangedCallback callback) override;
    void setGlyphAtlasCache(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) override;
    void setText(const std::u32string &value) override;
    void setText(std::u32string &&value) override;

    void eraseText(size_t startIndex)override;
    void eraseText(size_t startIndex, size_t count)override;
    void insertText(const std::u32string &text, size_t startIndex = 0)override;
    void appendText(const std::u32string &text)override;

private:
    void updateBuffers(size_t startFrom = 0);

    std::shared_ptr<IGlyphAtlasCache> _glyphAtlasCache;
    std::u32string _text{};

    struct UV {
        size_t i;
        float u;
        float v;
    };

    std::vector<UV> _uvs{};
    std::vector<glm::vec4> _transforms{};
    std::vector<uint32_t> _textAdvance{};
    bool _isDirty{false};
};
} // namespace rendell_text
