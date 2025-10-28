#pragma once
#include "TextLayout.h"
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

    void setText(std::wstring &&text);
    void setMatrix(const glm::mat4 &matrix);
    void setColor(const glm::vec4 &color);
    void setBackgroundColor(const glm::vec4 backgroundColor);

    const glm::vec4 &getColor() const;

    void draw(TextLayout &textLayout);

private:
    bool init();
    void setUniforms(glm::ivec2 fontSize);

    glm::mat4 _matrix{};
    glm::vec4 _color{};
    glm::vec4 _backgroundColor{};
};
} // namespace rendell_text
