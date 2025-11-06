#pragma once
#include <rendell_text/ITextLayout.h>
#include <rendell_text/ITextModel.h>

#include <glm/glm.hpp>
#include <vector>

namespace rendell_text {
class ITextModel;

class TextLayout final : public ITextLayout {
public:
    TextLayout(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache,
               std::shared_ptr<ITextModel> textModel);
    ~TextLayout() = default;

    bool isEmpty() const override;

    size_t getVersion() const override;
    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    std::shared_ptr<IGlyphAtlasCache> getGlyphAtlasCache() const override;

    void setGlyphAtlasCache(std::shared_ptr<IGlyphAtlasCache> glyphAtlasCache) override;
    void supplyText(const rendell_text::String &text) override;
    void setAutoSize(bool isAutoWith, bool isAutoHeight) override;
    //void setWorldWrap(bool isActive);

    void eraseText(size_t startIndex) override;
    void eraseText(size_t startIndex, size_t count) override;
    void insertText(const rendell_text::String &text, size_t startIndex = 0) override;
    void appendText(const rendell_text::String &text) override;

private:
    std::vector<LogicalLine> parseText(const rendell_text::String &text);
    VisualLine rasterizeString(const String &string);

    void updateBuffers(size_t startFrom = 0);

    size_t _version{0};

    bool _isAutoWith{};
    bool _isAutoHeight{};

    std::shared_ptr<ITextModel> _textModel;

    std::shared_ptr<IGlyphAtlasCache> _glyphAtlasCache;

    struct UV {
        size_t atlasIndex;
        float u;
        float v;
    };

    std::vector<UV> _uvs{};
    std::vector<glm::vec4> _transforms{};
    std::vector<uint32_t> _textAdvance{};
};
} // namespace rendell_text
