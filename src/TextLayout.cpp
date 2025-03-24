#include "TextLayout.h"
#include <fstream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "IFontRaster.h"
#include "RasteredFontStorageManager.h"

#define CHAR_RANGE_SIZE 200
#define TEXT_BUFFER_SIZE 100

const size_t CLEAR_BUFFER_CACHE_FLAG = 1 << 0;
const size_t UPDATE_BUFFER_FLAG = 1 << 1;

namespace rendell_text
{
	static std::unique_ptr<RasteredFontStorageManager> s_rasteredFontStorageManager;
	static uint32_t s_instanceCount{};
	static bool s_initialized = false;

	static rendell::VertexArraySharedPtr createVertexArray()
	{
		static std::vector<float> vertexPos{
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
		};

		rendell::VertexBufferSharedPtr vertexBuffer = rendell::createVertexBuffer(vertexPos);
		vertexBuffer->setLayouts({ { rendell::ShaderDataType::float2, false, 0 } });

		rendell::VertexArraySharedPtr vertexArray = rendell::createVertexArray();
		vertexArray->addVertexBuffer(vertexBuffer);

		return vertexArray;
	}

	static rendell::ShaderProgramSharedPtr createShaderProgram(std::string&& vertexSrc, std::string&& fragmentSrc)
	{
		rendell::ShaderProgramSharedPtr program = rendell::createShaderProgram(std::move(vertexSrc), std::move(fragmentSrc));

		if (std::string vertInfoLog, fragInfoLog; !program->compile(&vertInfoLog, &fragInfoLog))
		{
			std::cout << "ERROR::TextLayout: Shader compilation failure:\n" << vertInfoLog << fragInfoLog << std::endl;
			return nullptr;
		}
		if (std::string infoLog; !program->link(&infoLog))
		{
			std::cout << "ERROR::TextLayout: Shader linking failure:\n" << infoLog << std::endl;
			return nullptr;
		}
		return program;
	}

	static bool initStaticRendererStuff()
	{
		s_rasteredFontStorageManager.reset(new RasteredFontStorageManager);
		return true;
	}

	static void releaseStaticRendererStuff()
	{
		s_rasteredFontStorageManager.reset(nullptr);
		s_initialized = false;
	}

	TextLayout::TextLayout()
	{
		s_instanceCount++;
		init();
	}

	TextLayout::~TextLayout()
	{
		// Release it to check the cache.
		_rasteredFontStorage.reset();
		s_rasteredFontStorageManager->clearUnusedCache();

		s_instanceCount--;
		if (s_instanceCount == 0)
		{
			releaseStaticRendererStuff();
		}
	}

	bool TextLayout::isInitialized() const
	{
		return s_initialized;
	}

	const std::unordered_set<TextBatchSharedPtr>& TextLayout::getTextBatchesForRendering() const
	{
		return _textBatchesForRendering;
	}

	std::wstring TextLayout::getSubText(size_t indexFrom) const
	{
		assert(indexFrom < _text.length());
		return std::wstring(_text.begin() + indexFrom, _text.end());
	}

	void TextLayout::update()
	{
		updateBuffersIfNeeded();
	}

	void TextLayout::setFontPath(const std::filesystem::path& fontPath)
	{
		if (_fontPath != fontPath)
		{
			_fontPath = fontPath;
			_rasteredFontStorage = getRasteredFontStorage();
			_updateActionFlags |= CLEAR_BUFFER_CACHE_FLAG;
			_updateActionFlags |= UPDATE_BUFFER_FLAG;
		}
	}

	void TextLayout::setText(const std::wstring& value)
	{
		std::wstring text = value;
		setText(std::move(text));
	}

	void TextLayout::setText(std::wstring&& value)
	{
		_text = std::move(value);
		_updateActionFlags |= UPDATE_BUFFER_FLAG;
	}

	void TextLayout::setFontSize(const glm::ivec2& fontSize)
	{
		if (_fontSize != fontSize)
		{
			_fontSize = fontSize;
			_rasteredFontStorage = getRasteredFontStorage();
			_updateActionFlags |= CLEAR_BUFFER_CACHE_FLAG;
			_updateActionFlags |= UPDATE_BUFFER_FLAG;
		}
	}

	const std::filesystem::path& TextLayout::getFontPath() const
	{
		if (_rasteredFontStorage)
		{
			return _rasteredFontStorage->getFontRaster()->getFontPath();
		}

		return {};
	}

	glm::ivec2 TextLayout::getFontSize() const
	{
		return _fontSize;
	}

	const std::wstring& TextLayout::getText() const
	{
		return _text;
	}

	size_t TextLayout::getTextLength() const
	{
		return _text.length();
	}

	uint32_t TextLayout::getFontHeight() const
	{
		return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getFontHeight());
	}

	uint32_t TextLayout::getAscender() const
	{
		return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getAscender());
	}

	uint32_t TextLayout::getDescender() const
	{
		return static_cast<uint32_t>(_rasteredFontStorage->getFontRaster()->getDescender());
	}

	const std::vector<uint32_t>& TextLayout::getTextAdvance() const
	{
		updateBuffersIfNeeded();
		return _textAdvance;
	}

	void TextLayout::eraseText(uint32_t startIndex)
	{
		eraseText(startIndex, _text.length() - startIndex);
	}

	void TextLayout::eraseText(uint32_t startIndex, uint32_t count)
	{
		assert(startIndex >= 0 && startIndex + count <= _text.length());
		_text.erase(startIndex, count);
		_updateActionFlags |= UPDATE_BUFFER_FLAG;
	}

	void TextLayout::insertText(const std::wstring& text, uint32_t startIndex)
	{
		assert(startIndex >= 0 && startIndex <= _text.length());
		_text.insert(startIndex, text);
		_updateActionFlags |= UPDATE_BUFFER_FLAG;
	}

	void TextLayout::appendText(const std::wstring& text)
	{
		if (!text.empty())
		{
			_text += text;
			_updateActionFlags |= UPDATE_BUFFER_FLAG;
		}
	}

	bool TextLayout::init()
	{
		if (!s_initialized)
		{
			s_initialized = initStaticRendererStuff();
		}

		if (!s_initialized)
		{
			return false;
		}

		return s_initialized;
	}

	static glm::vec2 getInstanceLocalOffset(const RasterizedChar& rasterizedChar)
	{
		const glm::vec2 bearing = rasterizedChar.glyphBearing;
		const glm::vec2 size = rasterizedChar.glyphSize;
		return glm::vec2(bearing.x, bearing.y - size.y);
	}

	void TextLayout::updateShaderBuffers() const
	{
		_textBatchesForRendering.clear();
		_textAdvance.resize(_text.length());
		auto it = _textAdvance.begin();

		glm::vec2 currentOffset(0.0f, 0.0f);
		const size_t length = _text.length();
		for (size_t i = 0; i < length; i++)
		{
			const wchar_t currentCharacter = _text[i];

			if (currentCharacter == '\n')
			{
				currentOffset.x = 0.0f;
				currentOffset.y += _fontSize.y;
				continue;
			}

			const TextBatchSharedPtr& textBatch = createTextBatch(currentCharacter);
			if (!textBatch)
			{
				std::cout << "ERROR::TextLayout: Failed to create text batch" << std::endl;
				return;
			}

			if (_textBatchesForRendering.find(textBatch) == _textBatchesForRendering.end())
			{
				textBatch->beginUpdating();
				_textBatchesForRendering.insert(textBatch);
			}

			const RasterizedChar& rasterizedChar = textBatch->getGlyphBuffer()->getRasterizedChar(currentCharacter);

			if (currentCharacter != ' ' && currentCharacter != '\t')
			{
				const glm::vec2 glyphOffset = currentOffset + getInstanceLocalOffset(rasterizedChar);
				textBatch->appendCharacter(currentCharacter, glyphOffset);
			}

			currentOffset.x += (rasterizedChar.glyphAdvance >> 6);
			*it = currentOffset.x; it++;
		}

		for (const TextBatchSharedPtr& textBatch : _textBatchesForRendering)
		{
			textBatch->endUpdating();
		}
	}

	void TextLayout::updateBuffersIfNeeded() const
	{
		if (_updateActionFlags & CLEAR_BUFFER_CACHE_FLAG)
		{
			_rasteredFontStorage = getRasteredFontStorage();
			_cachedTextBatches.clear();
			_textBatchesForRendering.clear();
		}
		if (_updateActionFlags & UPDATE_BUFFER_FLAG)
		{
			updateShaderBuffers();
		}
		_updateActionFlags = 0;
	}

	RasteredFontStorageSharedPtr TextLayout::getRasteredFontStorage() const
	{
		RasteredFontStoragePreset preset{
			_fontPath.string(),
			_fontSize.x,
			_fontSize.y,
			CHAR_RANGE_SIZE,
		};
		const RasteredFontStorageSharedPtr result = s_rasteredFontStorageManager->getRasteredFontStorage(preset);
		s_rasteredFontStorageManager->clearUnusedCache();
		return result;
	}

	TextBatchSharedPtr TextLayout::createTextBatch(wchar_t character) const
	{
		const wchar_t rangeIndex = _rasteredFontStorage->getRangeIndex(character);
		if (auto it = _cachedTextBatches.find(rangeIndex); it != _cachedTextBatches.end())
		{
			return it->second;
		}

		GlyphBufferSharedPtr glyphBuffer = _rasteredFontStorage->rasterizeGlyphRange(rangeIndex);
		if (!glyphBuffer)
		{
			return nullptr;
		}
		const TextBatchSharedPtr result = makeTextBatch(glyphBuffer, TEXT_BUFFER_SIZE);;
		_cachedTextBatches[rangeIndex] = result;
		return result;
	}
}
