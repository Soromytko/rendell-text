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

    size_t getVersion() const override;
    std::shared_ptr<IGlyphAtlasCache> getGlyphAtlasCache() const override;
    const rendell_text::String &getText() const override;
    size_t getTextLength() const override;
    uint32_t getHeight() const override;
    uint32_t getAscender() const override;
    uint32_t getDescender() const override;
    const std::vector<uint32_t> &getTextAdvance() const override;
    size_t getTransformUnitSize() const override;
    size_t getUVUnitSize() const override;
    std::pair<const rendell::byte_t *, size_t> getTransforms() const override;
    std::pair<const rendell::byte_t *, size_t> getUVs() const override;

    rendell_text::String getSubText(size_t indexFrom) const override;

    void setGlyphAtlasCache(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) override;
    void setText(const rendell_text::String &value) override;
    void setText(rendell_text::String &&value) override;

    void eraseText(size_t startIndex) override;
    void eraseText(size_t startIndex, size_t count) override;
    void insertText(const rendell_text::String &text, size_t startIndex = 0) override;
    void appendText(const rendell_text::String &text) override;

private:
    void updateBuffers(size_t startFrom = 0);

    size_t _version{0};

    std::shared_ptr<IGlyphAtlasCache> _glyphAtlasCache;
    rendell_text::String _text{};

    struct UV {
        size_t i;
        float u;
        float v;
    };

    std::vector<UV> _uvs{};
    std::vector<glm::vec4> _transforms{};
    std::vector<uint32_t> _textAdvance{};
};
} // namespace rendell_text
