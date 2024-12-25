#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "GlyphBuffer.h"
#include "TextBuffer.h"

struct TextBatch
{
public:
	TextBatch(GlyphBufferSharedPtr glyphBuffer, size_t textBufferSize);
	~TextBatch() = default;

	void beginUpdating();
	void appendCharacter(wchar_t character, glm::vec2 offset);
	void endUpdating();

	const GlyphBuffer* getGlyphBuffer() const;
	const std::vector<std::unique_ptr<TextBuffer>>& GetTextBuffers() const;

private:
	size_t _counter{};
	size_t _textBufferSize{};

	GlyphBufferSharedPtr _glyphBuffer{};
	std::vector<std::unique_ptr<TextBuffer>> _textBuffers{};

};

typedef std::shared_ptr<TextBuffer> TextBatchSharedPtr;
