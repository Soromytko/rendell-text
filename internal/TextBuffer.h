#pragma once
#include <rendell/oop/ShaderBuffer.h>

#include <memory>

namespace rendell_text {
class ITextLayout;
} // namespace rendell_text

namespace rendell_text {
class TextBuffer final {
public:
    TextBuffer(std::shared_ptr<ITextLayout> textLayout);
    ~TextBuffer() = default;

    bool isEmtpy() const;
    size_t getLength() const;

    void setTextLayout(std::shared_ptr<ITextLayout> textLayout);

    void prepare();
    void use(uint32_t transformBinding, uint32_t uvBinding);

private:
    void updateBuffers();

    size_t _textLayoutVersion{0};
    bool _needsFullUpdate{false};
    std::shared_ptr<ITextLayout> _textLayout{};

    struct Buffers {
        std::unique_ptr<rendell::oop::ShaderBuffer> transformShaderBuffer{};
        std::unique_ptr<rendell::oop::ShaderBuffer> uvShaderBuffer{};

        size_t transformUnitSize{};
        size_t uvUnitSize{};

        bool updateSize(size_t textLength);

    private:
        size_t _textLength{};
    };

    Buffers _buffers{};
};
} // namespace rendell_text