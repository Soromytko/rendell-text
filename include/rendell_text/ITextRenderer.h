#pragma once
#include <memory>

#include <glm/glm.hpp>

namespace rendell_text {
class ITextLayout;
} // namespace rendell_text

namespace rendell_text {
class ITextRenderer {
public:
    ITextRenderer() = default;
    virtual ~ITextRenderer() = default;

    virtual std::shared_ptr<ITextLayout> getTextLayout() const = 0;
    virtual const glm::vec4 &getColor() const = 0;

    virtual void setMatrix(const glm::mat4 &matrix) = 0;
    virtual void setColor(const glm::vec4 &color) = 0;
    virtual void setBackgroundColor(const glm::vec4 backgroundColor) = 0;

    virtual void draw() = 0;
};
} // namespace rendell_text