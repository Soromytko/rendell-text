#include "TextRenderer.h"
#include <fstream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "IFontRaster.h"
#include "RasteredFontStorageManager.h"
#include "res_Shaders_TextRenderer_vs.h"
#include "res_Shaders_TextRenderer_fs.h"

#define TEXTURE_ARRAY_BLOCK 0

#define UNIFORM_BUFFER_BINDING 0
#define TEXT_BUFFER_BINDING 1
#define GLYPH_TRANSFORM_BUFFER_BINDING 2

namespace rendell_text
{
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
		vertSrcResult = res_Shaders_TextRenderer_vs;
		fragSrcResult = res_Shaders_TextRenderer_fs;

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

	const TextLayoutSharedPtr& TextRenderer::getTextLayout() const
	{
		return _textLayout;
	}

	void TextRenderer::setTextLayout(const TextLayoutSharedPtr& textLayout)
	{
		_textLayout = textLayout;
	}

	void TextRenderer::setMatrix(const glm::mat4& matrix)
	{
		_matrix = matrix;
	}

	void TextRenderer::setColor(const glm::vec4& color)
	{
		_color = color;
	}

	void TextRenderer::setBackgroundColor(const glm::vec4 backgroundColor)
	{
		_backgroundColor = backgroundColor;
	}

	const glm::vec4& TextRenderer::getColor() const
	{
		return _color;
	}

	void TextRenderer::draw()
	{
		if (!_textLayout || _textLayout->getText().length() == 0) {
			return;
		}

		_textLayout->update();

		beginDrawing();

		for (const TextBatchSharedPtr& textBatch : _textLayout->getTextBatchesForRendering())
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

	void TextRenderer::beginDrawing()
	{
		s_shaderProgram->bind();
		s_vertexArray->bind();

		const glm::ivec2 fontSize = _textLayout->getFontSize();

		s_shaderProgram->setUniformMat4(s_matrixUniformIndex, reinterpret_cast<const float*>(&_matrix));
		s_shaderProgram->setUniformFloat2(s_fontSizeUniformIndex, fontSize.x, fontSize.y);
		s_shaderProgram->setUniformFloat4(s_colorUniformIndex, _color.r, _color.g, _color.b, _color.a);
		s_shaderProgram->setUniformVec4(s_backgroundColorUniformIndex, reinterpret_cast<const float*>(&_backgroundColor));
	}

	void TextRenderer::endDrawing()
	{
		s_shaderProgram->unbind();
		s_vertexArray->unbind();
	}
}
