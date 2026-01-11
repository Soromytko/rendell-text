#pragma once

namespace rendell_text {
class ITextLayout;

class ITextBuffer {
public:
    ITextBuffer() = default;
    virtual ~ITextBuffer() = default;

    virtual bool isEmtpy() const = 0;
    virtual size_t getLength() const = 0;
    virtual std::shared_ptr<ITextLayout> getTextLayout() const = 0;

    virtual void setTextLayout(std::shared_ptr<ITextLayout> textLayout) = 0;

    virtual size_t update(ITextLayout &textLayout, size_t offset = 0) = 0;
    virtual void prepare() = 0;
    virtual void use(uint32_t transformBinding, uint32_t uvBinding) = 0;
};
} // namespace rendell_text