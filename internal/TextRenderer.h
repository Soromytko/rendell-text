#pragma once
#include <rendell_text/ITextRenderer.h>

#include <memory>

namespace rendell_text {
class TextRenderer final : public ITextRenderer {
public:
    static bool initBasicRenderResources();
    static void releaseBasicRenderResources();

    TextRenderer(std::shared_ptr<ITextBuffer> textBuffer,
                 std::shared_ptr<IGlyphAtlasTexture> atlasTexture);
    ~TextRenderer() = default;

    std::shared_ptr<ITextBuffer> getTextBuffer() const override;
    std::shared_ptr<IGlyphAtlasTexture> getGlyphAtlasTexture() const override;
    const glm::vec4 &getColor() const override;

    void setTextBuffer(std::shared_ptr<ITextBuffer> textBuffer) override;
    void setGlyphAtlasTexture(std::shared_ptr<IGlyphAtlasTexture> atlasTexture) override;
    void setMatrix(const glm::mat4 &matrix) override;
    void setColor(const glm::vec4 &color) override;
    void setBackgroundColor(const glm::vec4 backgroundColor) override;

    void prepare() override;
    void draw() override;

private:
    std::shared_ptr<ITextBuffer> _textBuffer{};
    std::shared_ptr<IGlyphAtlasTexture> _atlasTexture{};

    glm::mat4 _matrix{};
    glm::vec4 _color{};
    glm::vec4 _backgroundColor{};
};
} // namespace rendell_text
