#pragma once
#include <rendell_text/ITextRenderer.h>

#include <memory>

namespace rendell_text {
class TextBuffer;
class IGlyphAtlasCache;
class GlyphAtlasTexture;
} // namespace rendell_text

namespace rendell_text {
class TextRenderer final : public ITextRenderer {
public:
    static bool initStaticStuff();
    static void releaseStaticStuff();

    TextRenderer(std::shared_ptr<ITextLayout> textLayout);
    ~TextRenderer() = default;

    std::shared_ptr<ITextLayout> getTextLayout() const override;
    const glm::vec4 &getColor() const override;

    void setMatrix(const glm::mat4 &matrix) override;
    void setColor(const glm::vec4 &color) override;
    void setBackgroundColor(const glm::vec4 backgroundColor) override;

    void prepare();
    void draw() override;

private:
    void setTextLayout(std::shared_ptr<ITextLayout> textLayout);

    std::shared_ptr<IGlyphAtlasCache> _glyphAtlasCache{};
    std::shared_ptr<ITextLayout> _textLayout{};
    std::shared_ptr<TextBuffer> _textBuffer{};

    std::shared_ptr<GlyphAtlasTexture> _atlasTextures{};

    glm::mat4 _matrix{};
    glm::vec4 _color{};
    glm::vec4 _backgroundColor{};
};
} // namespace rendell_text
