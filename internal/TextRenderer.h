#pragma once
#include <rendell/oop/Texture2DArray.h>
#include <rendell_text/ITextRenderer.h>k

#include <GlyphAtlasTexture.h>

#include <memory>

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

    void setUniforms();

    std::shared_ptr<ITextLayout> _textLayout{};
    std::shared_ptr<GlyphAtlasCache> _glyphAtlasCache{};
    glm::mat4 _matrix{};
    glm::vec4 _color{};
    glm::vec4 _backgroundColor{};

    std::shared_ptr<GlyphAtlasTexture> _atlasTextures{};
};

RENDELL_USE_RAII_FACTORY(TextRenderer)
} // namespace rendell_text
