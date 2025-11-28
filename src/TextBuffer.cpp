#include <TextBuffer.h>

#include <rendell_text/ITextLayout.h>

#include <cassert>

namespace rendell_text {
TextBuffer::TextBuffer(std::shared_ptr<ITextLayout> textLayout) {
    assert(textLayout);
    setTextLayout(textLayout);
}

size_t TextBuffer::getLength() const {
    #error
    //return 
}

void TextBuffer::prepare() {
    assert(_textLayout);

    if (!_isDirty) {
        return;
    }

    _isDirty = false;
}

void TextBuffer::setTextLayout(std::shared_ptr<ITextLayout> textLayout) {
    assert(textLayout);
    if (_textLayout == textLayout) {
        return;
    }
    _textLayout = textLayout;

    _textLayout->setChangedCallback([this]() { _isDirty = true; });
}
} // namespace rendell_text
