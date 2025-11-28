#pragma once
#include <glm/glm.hpp>
#include <memory>

namespace rendell_text {
class ITextLayout;
} // namespace rendell_text

namespace rendell_text {
class TextBuffer final {
public:
    TextBuffer(std::shared_ptr<ITextLayout> textLayout);
    ~TextBuffer() = default;

    size_t getLength() const;

    void prepare();

    void setTextLayout(std::shared_ptr<ITextLayout> textLayout);

private:
    std::shared_ptr<ITextLayout> _textLayout{};

    bool _isDirty{false};
};
} // namespace rendell_text