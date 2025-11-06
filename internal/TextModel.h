#pragma once
#include <rendell_text/ITextModel.h>

namespace rendell_text {
class TextModel final : public ITextModel {
public:
    TextModel() = default;
    ~TextModel() = default;

    bool isEmpty() const override;
    size_t getRowCount() const override;

    virtual void setText(std::vector<LogicalLine> lines) override;
    virtual void insert(size_t rowIndex, size_t columnIndex, const GlyphBuffer &glyphBuffer) override;
    virtual void erase(size_t logicalLineIndex, size_t columnIndex, size_t count) override;
    virtual void eraseLogicalLine(size_t logicalLineIndex) override;


private:
};

} // namespace rendell_text