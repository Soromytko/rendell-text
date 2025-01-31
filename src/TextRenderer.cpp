#include "TextRenderer.h"
#include <fstream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "IFontRaster.h"
#include "RasteredFontStorageManager.h"

#define CHAR_RANGE_SIZE 200
#define TEXT_BUFFER_SIZE 100

#define TEXTURE_ARRAY_BLOCK 0

#define UNIFORM_BUFFER_BINDING 0
#define TEXT_BUFFER_BINDING 1
#define GLYPH_TRANSFORM_BUFFER_BINDING 2

namespace rendell_text
{
	static const char* s_vertShaderFilePath = "../res/Shaders/TextRenderer.vs";
	static const char* s_fragShaderFilePath = "../res/Shaders/TextRenderer.fs";

	static rendell::VertexArraySharedPtr s_vertexArray;
	static rendell::ShaderProgramSharedPtr s_shaderProgram;
	static std::unique_ptr<RasteredFontStorageManager> s_rasteredFontStorageManager;
	static uint32_t s_matrixUniformIndex{};
	static uint32_t s_fontSizeUniformIndex{};
	static uint32_t s_colorUniformIndex{};
	static uint32_t s_backgroundColorUniformIndex{};
	static uint32_t s_charFromUniformIndex{};
	static uint32_t s_textureArrayUniformIndex{};
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
			std::cout << "ERROR::TextRenderer: Shader compilation failure:\n" << vertInfoLog << fragInfoLog << std::endl;
			return nullptr;
		}
		if (std::string infoLog; !program->link(&infoLog))
		{
			std::cout << "ERROR::TextRenderer: Shader linking failure:\n" << infoLog << std::endl;
			return nullptr;
		}
		return program;
	}

	static bool loadShaders(std::string& vertSrcResult, std::string& fragSrcResult)
	{
		std::fstream vertStream(s_vertShaderFilePath);
		if (!vertStream.is_open())
		{
			std::cout << "ERROR::TextRenderer: Failed to open file " << s_vertShaderFilePath << std::endl;
			return false;
		}

		std::fstream fragStream(s_fragShaderFilePath);
		if (!fragStream.is_open())
		{
			std::cout << "ERROR::TextRenderer: Failed to open file " << s_fragShaderFilePath << std::endl;
			return false;
		}

		std::string vertexSrc((std::istreambuf_iterator<char>(vertStream)), std::istreambuf_iterator<char>());
		std::string fragmentSrc((std::istreambuf_iterator<char>(fragStream)), std::istreambuf_iterator<char>());

		vertSrcResult = std::move(vertexSrc);
		fragSrcResult = std::move(fragmentSrc);

		return true;
	}

	static bool initStaticRendererStuff()
	{
		s_rasteredFontStorageManager.reset(new RasteredFontStorageManager);

		s_vertexArray = createVertexArray();

		std::string vertexSrc, fragmentSrc;
		if (!loadShaders(vertexSrc, fragmentSrc))
		{
			std::cout << "ERROR::TextRenderer: Shader loading failed" << std::endl;
			return false;
		}

		s_shaderProgram = createShaderProgram(std::move(vertexSrc), std::move(fragmentSrc));

		if (s_vertexArray != nullptr && s_shaderProgram != nullptr)
		{
			s_shaderProgram->setShaderBufferBinding("textBuffer", TEXT_BUFFER_BINDING);
			s_shaderProgram->setShaderBufferBinding("glyphTransformBuffer", GLYPH_TRANSFORM_BUFFER_BINDING);

			s_matrixUniformIndex = s_shaderProgram->getUniformIndex("u_Matrix");
			s_fontSizeUniformIndex = s_shaderProgram->getUniformIndex("u_FontSize");
			s_colorUniformIndex = s_shaderProgram->getUniformIndex("u_TextColor");
			s_backgroundColorUniformIndex = s_shaderProgram->getUniformIndex("u_BackgroundColor");
			s_charFromUniformIndex = s_shaderProgram->getUniformIndex("u_CharFrom");
			s_textureArrayUniformIndex = s_shaderProgram->getUniformIndex("u_Textures");

			s_shaderProgram->setUniformInt1(s_textureArrayUniformIndex, TEXTURE_ARRAY_BLOCK);

			return true;
		}

		return false;
	}

	static void releaseStaticRendererStuff()
	{
		s_rasteredFontStorageManager.reset(nullptr);
		s_vertexArray.reset();
		s_shaderProgram.reset();
		s_initialized = false;
	}

	TextRenderer::TextRenderer()
	{
		s_instanceCount++;
		init();
	}

	TextRenderer::~TextRenderer()
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

	bool TextRenderer::isInitialized() const
	{
		return s_initialized;
	}

	void TextRenderer::setFontPath(const std::filesystem::path& fontPath)
	{
		_rasteredFontStorage = getRasteredFontStorage(fontPath);
	}

	void TextRenderer::setText(const std::wstring& value)
	{
		std::wstring text = value;
		setText(std::move(text));
	}

	void TextRenderer::setText(std::wstring&& value)
	{
		_text = std::move(value);
		_shouldBuffersBeUpdated = true;
	}

	void TextRenderer::setMatrix(const glm::mat4& matrix)
	{
		_matrix = matrix;
	}

	void TextRenderer::setFontSize(const glm::ivec2& fontSize)
	{
		_fontSize = fontSize;
		if (_rasteredFontStorage)
		{
			const std::filesystem::path& fontPath = _rasteredFontStorage->getFontRaster()->getFontPath();
			_rasteredFontStorage = getRasteredFontStorage(fontPath);
			_shouldTextBatchBeUpdated = true;
			_shouldBuffersBeUpdated = true;
		}
	}

	void TextRenderer::setColor(const glm::vec4& color)
	{
		_color = color;
	}

	void TextRenderer::setBackgroundColor(const glm::vec4 backgroundColor)
	{
		_backgroundColor = backgroundColor;
	}

	const std::filesystem::path& TextRenderer::getFontPath() const
	{
		if (_rasteredFontStorage)
		{
			return _rasteredFontStorage->getFontRaster()->getFontPath();
		}

		return {};
	}

	glm::ivec2 TextRenderer::getFontSize() const
	{
		return _fontSize;
	}

	const std::wstring& TextRenderer::getText() const
	{
		return _text;
	}

	const glm::vec4& TextRenderer::getColor() const
	{
		return _color;
	}

	const std::vector<uint32_t>& TextRenderer::getTextAdvance() const
	{
		return _textAdvance;
	}

	void TextRenderer::draw()
	{
		if (_text.length() == 0) {
			return;
		}

		updateBuffersIfNeeded();

		beginDrawing();

		for (const TextBatch* textBatch : _textBatchesForRendering)
		{
			const GlyphBuffer* glyphBuffer = textBatch->getGlyphBuffer();
			glyphBuffer->bind(TEXTURE_ARRAY_BLOCK);
			s_shaderProgram->setUniformInt1(s_charFromUniformIndex, glyphBuffer->getRange().first);
			for (const std::unique_ptr<TextBuffer>& textBuffer : textBatch->GetTextBuffers())
			{
				textBuffer->bind(TEXT_BUFFER_BINDING, GLYPH_TRANSFORM_BUFFER_BINDING);
				rendell::drawTriangleStripArraysInstanced(0, 4, static_cast<uint32_t>(textBuffer->getCurrentLength()));
				textBuffer->unbind();
			}
			glyphBuffer->unbind();
		}

		endDrawing();
	}

	bool TextRenderer::init()
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

	void TextRenderer::updateShaderBuffers()
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

			TextBatch* textBatch = createTextBatch(currentCharacter);
			if (!textBatch)
			{
				std::cout << "ERROR::TextRenderer: Failed to create text batch" << std::endl;
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

			*it = (rasterizedChar.glyphAdvance >> 6); it++;
			currentOffset.x += (rasterizedChar.glyphAdvance >> 6);
		}

		for (TextBatch* textBatch : _textBatchesForRendering)
		{
			textBatch->endUpdating();
		}
	}

	void TextRenderer::beginDrawing()
	{
		s_shaderProgram->bind();
		s_vertexArray->bind();

		s_shaderProgram->setUniformMat4(s_matrixUniformIndex, reinterpret_cast<const float*>(&_matrix));
		s_shaderProgram->setUniformFloat2(s_fontSizeUniformIndex, _fontSize.x, _fontSize.y);
		s_shaderProgram->setUniformFloat4(s_colorUniformIndex, _color.r, _color.g, _color.b, _color.a);
		s_shaderProgram->setUniformVec4(s_backgroundColorUniformIndex, reinterpret_cast<const float*>(&_backgroundColor));
	}

	void TextRenderer::endDrawing()
	{
		s_shaderProgram->unbind();
		s_vertexArray->unbind();
	}

	void TextRenderer::updateBuffersIfNeeded()
	{
		if (_shouldBuffersBeUpdated)
		{
			if (_shouldTextBatchBeUpdated)
			{
				_textBatchesForRendering.clear();
				_shouldTextBatchBeUpdated = false;
			}
			_textBatches.clear();
			updateShaderBuffers();
			_shouldBuffersBeUpdated = false;
		}
	}

	RasteredFontStorageSharedPtr TextRenderer::getRasteredFontStorage(const std::filesystem::path& fontPath)
	{
		RasteredFontStoragePreset preset{
			fontPath.string(),
			_fontSize.x,
			_fontSize.y,
			CHAR_RANGE_SIZE,
		};
		return s_rasteredFontStorageManager->getRasteredFontStorage(preset);
	}

	TextBatch* TextRenderer::createTextBatch(wchar_t character)
	{
		const wchar_t rangeIndex = _rasteredFontStorage->getRangeIndex(character);
		if (auto it = _textBatches.find(rangeIndex); it != _textBatches.end())
		{
			return it->second.get();
		}

		GlyphBufferSharedPtr glyphBuffer = _rasteredFontStorage->rasterizeGlyphRange(rangeIndex);
		if (!glyphBuffer)
		{
			return nullptr;
		}
		TextBatch* textBatch = new TextBatch(glyphBuffer, TEXT_BUFFER_SIZE);
		_textBatches[rangeIndex] = std::unique_ptr<TextBatch>(textBatch);

		return textBatch;
	}
}
