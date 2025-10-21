#pragma once
#include "TextLayout.h"
#include "defines.h"
#include "private/RasteredFontStorage.h"
#include "private/TextBatch.h"
#include <glm/glm.hpp>
#include <map>
#include <rendell/rendell.h>
#include <unordered_set>

namespace rendell_text {
class TextRenderer final {
public:
    TextRenderer();
    ~TextRenderer();

    bool isInitialized() const;
    const TextLayoutSharedPtr &getTextLayout() const;

    void setTextLayout(const TextLayoutSharedPtr &textLayout);
    void setMatrix(const glm::mat4 &matrix);
    void setColor(const glm::vec4 &color);
    void setBackgroundColor(const glm::vec4 backgroundColor);

    const glm::vec4 &getColor() const;

    void draw();

private:
    bool init();
    void setUniforms();

    TextLayoutSharedPtr _textLayout{};
    glm::mat4 _matrix{};
    glm::vec4 _color{};
    glm::vec4 _backgroundColor{};
};

RENDELL_TEXT_DECLARE_SHARED_PTR_FACTORY(TextRenderer)
} // namespace rendell_text
