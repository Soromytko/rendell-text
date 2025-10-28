#pragma once
#include "private/RasteredFontStorage.h"
#include "private/TextBatch.h"

#include <glm/glm.hpp>
#include <map>
#include <rendell/rendell.h>
#include <unordered_set>

namespace rendell_text {
class TextLayout final {
public:
    TextLayout();
    TextLayout(TextLayout &&other) noexcept;
    ~TextLayout();

    bool isInitialized() const;
    const std::unordered_set<TextBatchSharedPtr> &getTextBatchesForRendering() const;
    std::wstring getSubText(size_t indexFrom) const;

    void beginUpdating();

    void setFontPath(const std::filesystem::path &fontPath);
    void setText(const std::wstring &value);
    void setText(std::wstring &&value);
    void setFontSize(const glm::ivec2 &fontSize);

    void endUpdating();

    inline bool isUpdating() const noexcept { return _isUpdating; };

    bool isEmpty() const;
    const std::filesystem::path &getFontPath() const;
    glm::ivec2 getFontSize() const;
    const std::wstring &getText() const;
    size_t getTextLength() const;
    uint32_t getFontHeight() const;
    uint32_t getAscender() const;
    uint32_t getDescender() const;
    const std::vector<uint32_t> &getTextAdvance() const;

    void eraseText(size_t startIndex);
    void eraseText(size_t startIndex, size_t count);
    void insertText(const std::wstring &text, size_t startIndex = 0);
    void appendText(const std::wstring &text);

private:
    bool init();

    void updateShaderBuffers();
    void updateBuffersIfNeeded();

    RasteredFontStorageSharedPtr getRasteredFontStorage() const;

    TextBatchSharedPtr createTextBatch(wchar_t character);

    glm::ivec2 _fontSize;
    std::filesystem::path _fontPath{};
    std::wstring _text{};

    bool _isUpdating{false};

    RasteredFontStorageSharedPtr _rasteredFontStorage{nullptr};
    std::map<uint32_t, TextBatchSharedPtr> _cachedTextBatches{};
    std::unordered_set<TextBatchSharedPtr> _textBatchesForRendering{};
    std::vector<uint32_t> _textAdvance{};
    size_t _updateActionFlags{};
};
} // namespace rendell_text
