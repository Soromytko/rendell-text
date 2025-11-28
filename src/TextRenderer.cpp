#include <TextRenderer.h>

#include <GlyphAtlasTexture.h>
#include <TextBuffer.h>
#include <rendell/oop/rendell_oop.h>
#include <rendell_text/ITextLayout.h>

#include "RasteredFontStorageManager.h"
#include "res_Shaders_TextRenderer_fs.h"
#include "res_Shaders_TextRenderer_vs.h"
#include <GlyphAtlasTextureStorage.h>
#include <logging.h>

#include <cassert>
#include <glm/gtc/type_ptr.hpp>

#define TEXTURE_ARRAY_BLOCK 0
#define TEXT_BUFFER_BINDING 0
#define GLYPH_TRANSFORM_BUFFER_BINDING 1

namespace rendell_text {
static rendell::oop::VertexAssemblySharedPtr s_vertexAssembly;
static rendell::oop::ShaderProgramSharedPtr s_shaderProgram;
static std::unique_ptr<RasteredFontStorageManager> s_rasteredFontStorageManager;
static std::unique_ptr<rendell::oop::Mat4Uniform> s_matrixUniform{nullptr};
static std::unique_ptr<rendell::oop::Float2Uniform> s_fontSizeUniform{nullptr};
static std::unique_ptr<rendell::oop::Float4Uniform> s_textColorUniform{nullptr};
static std::unique_ptr<rendell::oop::Float4Uniform> s_backgroundColorUniform{nullptr};
static std::unique_ptr<rendell::oop::Int1Uniform> s_charFromUniformUniform{nullptr};
static std::unique_ptr<rendell::oop::Sampler2DUniform> s_texturesUniform{nullptr};

static rendell::oop::VertexAssemblySharedPtr createVertexAssembly() {
    static std::vector<float> vertexPos{
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    };
    static std::vector<uint32_t> indices{0, 0, 0, 0};

    auto indexBuffer = rendell::oop::makeIndexBuffer(indices.data(), indices.size());
    auto vertexBuffer = rendell::oop::makeVertexBuffer(vertexPos.data(), vertexPos.size());
    auto vertexLayout =
        rendell::VertexLayout().addAttribute(0, rendell::ShaderDataType::float2, false, 0);
    auto vertexAssembly = rendell::oop::makeVertexAssembly(indexBuffer, std::vector{vertexBuffer},
                                                           std::vector{vertexLayout});
    return vertexAssembly;
}

static rendell::oop::ShaderProgramSharedPtr createShaderProgram(const std::string &vertexSrc,
                                                                const std::string &fragmentSrc) {
    auto vertexShader =
        rendell::oop::makeVertexShader(vertexSrc, [](bool success, const std::string &infoLog) {
            if (!infoLog.empty()) {
                if (success) {
                    RT_WARNING("Vertex shader compilation warning:\n{}", infoLog);
                } else {
                    RT_CRITICAL("Vertex shader compilation error:\n{}", infoLog);
                }
            }
            assert(success);
        });

    auto fragmentShader =
        rendell::oop::makeFragmentShader(fragmentSrc, [](bool success, const std::string &infoLog) {
            if (!infoLog.empty()) {
                if (success) {
                    RT_WARNING("Fragment shader compilation warning:\n{}", infoLog);
                } else {
                    RT_CRITICAL("Fragment shader compilation error:\n{}", infoLog);
                }
            }
            assert(success);
        });

    auto program = rendell::oop::makeShaderProgram(
        vertexShader, fragmentShader, [](bool success, const std::string &infoLog) {
            if (!infoLog.empty()) {
                if (success) {
                    RT_WARNING("Shader program linking warning:\n{}", infoLog);
                } else {
                    RT_CRITICAL("Shader program linking error:\n{}", infoLog);
                }
            }
            assert(success);
        });

    return program;
}

static bool loadShaders(std::string &vertSrcResult, std::string &fragSrcResult) {
    vertSrcResult = res_Shaders_TextRenderer_vs;
    fragSrcResult = res_Shaders_TextRenderer_fs;

    return true;
}

bool TextRenderer::initStaticStuff() {
    s_rasteredFontStorageManager.reset(new RasteredFontStorageManager);

    s_vertexAssembly = createVertexAssembly();
    assert(s_vertexAssembly);

    std::string vertexSrc, fragmentSrc;
    if (!loadShaders(vertexSrc, fragmentSrc)) {
        RT_ERROR("Shader loading failed");
        return false;
    }

    s_shaderProgram = createShaderProgram(vertexSrc, fragmentSrc);
    assert(s_shaderProgram);

    s_matrixUniform = std::make_unique<rendell::oop::Mat4Uniform>("u_Matrix");
    s_fontSizeUniform = std::make_unique<rendell::oop::Float2Uniform>("u_FontSize");
    s_textColorUniform = std::make_unique<rendell::oop::Float4Uniform>("u_TextColor");
    s_backgroundColorUniform = std::make_unique<rendell::oop::Float4Uniform>("u_BackgroundColor");
    s_charFromUniformUniform = std::make_unique<rendell::oop::Int1Uniform>("u_CharFrom");
    s_texturesUniform = std::make_unique<rendell::oop::Sampler2DUniform>("u_Textures");

    return true;
}

void TextRenderer::releaseStaticStuff() {
    s_rasteredFontStorageManager.reset(nullptr);
    s_vertexAssembly.reset();
    s_shaderProgram.reset();
    s_matrixUniform.reset();
    s_fontSizeUniform.reset();
    s_textColorUniform.reset();
    s_backgroundColorUniform.reset();
    s_charFromUniformUniform.reset();
    s_texturesUniform.reset();
}

TextRenderer::TextRenderer(std::shared_ptr<ITextLayout> textLayout) {
    assert(textLayout);
    setTextLayout(textLayout);
}

std::shared_ptr<ITextLayout> TextRenderer::getTextLayout() const {
    return _textLayout;
}

const glm::vec4 &TextRenderer::getColor() const {
    return _color;
}

void TextRenderer::setTextLayout(std::shared_ptr<ITextLayout> textLayout) {
    assert(textLayout);
    if (_textLayout != textLayout) {
        _textLayout = textLayout;
        _textBuffer = std::make_shared<TextBuffer>(_textLayout);
    }
}

void TextRenderer::setMatrix(const glm::mat4 &matrix) {
    _matrix = matrix;
}

void TextRenderer::setColor(const glm::vec4 &color) {
    _color = color;
}

void TextRenderer::setBackgroundColor(const glm::vec4 backgroundColor) {
    _backgroundColor = backgroundColor;
}

void TextRenderer::prepare() {
    assert(_textLayout);
    assert(_textBuffer);

    auto glyphAtlasCache = _textLayout->getGlyphAtlasCache();
    assert(glyphAtlasCache);
    if (_glyphAtlasCache != glyphAtlasCache) {
        _glyphAtlasCache = glyphAtlasCache;
        _atlasTextures =
            GlyphAtlasTextureStorage::getInstance()->getOrCreateAtlasTexture(_glyphAtlasCache);
        assert(_atlasTextures);
    }
    assert(_atlasTextures);
    _atlasTextures->prepare();

    _textBuffer->prepare();
}

void TextRenderer::draw() {
    assert(_textLayout);
    assert(_atlasTextures);
    assert(_textBuffer);

    prepare();

    if (_textLayout->isEmpty() == 0) {
        return;
    }

    assert(_textBuffer->getLength() == _textLayout->getText().length());

    s_shaderProgram->use();
    s_vertexAssembly->use();

    const auto transformsBuffer = _textBuffer->getTransforms();
    const auto textureArray = _atlasTextures->getTextureArray();

    s_shaderProgram->use();
    s_vertexAssembly->use();
    transformsBuffer->use(s_transformUnifom->getId(), GLYPH_TRANSFORM_BUFFER_BINDING);
    textureArray->use(s_texturesUniform->getId(), TEXTURE_ARRAY_BLOCK);

    s_matrixUniform->set(glm::value_ptr(_matrix));
    s_fontSizeUniform->set(static_cast<float>(fontSize.x), static_cast<float>(fontSize.y));
    s_textColorUniform->set(_color.r, _color.g, _color.b, _color.a);
    s_backgroundColorUniform->set(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b,
                                  _backgroundColor.a);

    rendell::setDrawType(rendell::DrawMode::ArraysInstanced,
                         rendell::PrimitiveTopology::TriangleStrip,
                         static_cast<uint32_t>(_textBuffer->getLength()));
    rendell::submit();
}

} // namespace rendell_text
