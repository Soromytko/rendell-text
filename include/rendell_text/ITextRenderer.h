#pragma once
#include <memory>

#include <glm/glm.hpp>

namespace rendell_text {
class ITextBuffer;
class IGlyphAtlasTexture;
} // namespace rendell_text

namespace rendell_text {
class ITextRenderer {
public:
    ITextRenderer() = default;
    virtual ~ITextRenderer() = default;

    virtual std::shared_ptr<ITextBuffer> getTextBuffer() const = 0;
    virtual std::shared_ptr<IGlyphAtlasTexture> getGlyphAtlasTexture() const = 0;
    virtual const glm::vec4 &getColor() const = 0;

    virtual void setTextBuffer(std::shared_ptr<ITextBuffer> textBuffer) = 0;
    virtual void setGlyphAtlasTexture(std::shared_ptr<IGlyphAtlasTexture> atlasTexture) = 0;
    virtual void setMatrix(const glm::mat4 &matrix) = 0;
    virtual void setColor(const glm::vec4 &color) = 0;
    virtual void setBackgroundColor(const glm::vec4 backgroundColor) = 0;

    virtual void prepare() = 0;
    virtual void draw() = 0;
};
} // namespace rendell_text