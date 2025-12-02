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
    // return
}

void TextBuffer::prepare() {
    assert(_textLayout);

    if (_needsFullUpdate) {
        _textLayoutVersion = _textLayout->getVersion();
        _needsFullUpdate = false;
        return;
    }

    if (_textLayoutVersion != _textLayout->getVersion()) {
#error update buffers
        _textLayoutVersion = _textLayout->getVersion();
    }
}

void TextBuffer::setTextLayout(std::shared_ptr<ITextLayout> textLayout) {
    assert(textLayout);
    if (_textLayout != textLayout) {
        _textLayout = textLayout;
        _needsFullUpdate = true;
    }
}
} // namespace rendell_text
