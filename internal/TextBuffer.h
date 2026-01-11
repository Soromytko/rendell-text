#pragma once
#include <rendell/oop/ShaderBuffer.h>
#include <rendell_text/ITextBuffer.h>

#include <memory>

namespace rendell_text {
class TextBuffer final : public ITextBuffer {
public:
    TextBuffer(size_t transformUnitSize, size_t uvUnitSize, size_t count = 1024);
    ~TextBuffer() = default;

    bool isEmtpy() const override;
    size_t getLength() const override;

    void setTextLayout(std::shared_ptr<ITextLayout> textLayout) override;

    size_t update(ITextLayout &textLayout, size_t offset = 0) override;
    void prepare() override;
    void use(uint32_t transformBinding, uint32_t uvBinding) override;

private:
    void updateBuffers();

    size_t _count{0};
    size_t _textLayoutVersion{0};
    bool _needsFullUpdate{false};
    std::shared_ptr<ITextLayout> _textLayout{};

    std::unique_ptr<rendell::oop::ShaderBuffer> _transformShaderBuffer{};
    std::unique_ptr<rendell::oop::ShaderBuffer> _uvShaderBuffer{};

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