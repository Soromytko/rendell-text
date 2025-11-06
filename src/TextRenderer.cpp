#include <rendell_text/TextRenderer.h>

#include "RasteredFontStorageManager.h"
#include "res_Shaders_TextRenderer_fs.h"
#include "res_Shaders_TextRenderer_vs.h"
#include <logging.h>
#include <rendell_text/private/IFontRaster.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <memory>

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
static uint32_t s_instanceCount{};
static bool s_initialized = false;

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

static bool initStaticRendererStuff() {
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

static void releaseStaticRendererStuff() {
    s_rasteredFontStorageManager.reset(nullptr);
    s_vertexAssembly.reset();
    s_shaderProgram.reset();
    s_matrixUniform.reset();
    s_fontSizeUniform.reset();
    s_textColorUniform.reset();
    s_backgroundColorUniform.reset();
    s_charFromUniformUniform.reset();
    s_texturesUniform.reset();

    s_initialized = false;
}

TextRenderer::TextRenderer() {
    s_instanceCount++;
    init();
}

TextRenderer::~TextRenderer() {
    s_instanceCount--;
    if (s_instanceCount == 0) {
        releaseStaticRendererStuff();
    }
}

bool TextRenderer::isInitialized() const {
    return s_initialized;
}

const TextLayoutSharedPtr &TextRenderer::getTextLayout() const {
    return _textLayout;
}

void TextRenderer::setTextLayout(const TextLayoutSharedPtr &textLayout) {
    _textLayout = textLayout;
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

const glm::vec4 &TextRenderer::getColor() const {
    return _color;
}

void TextRenderer::draw() {
    if (!_textLayout || _textLayout->getText().length() == 0) {
        return;
    }

    _textLayout->update();

    for (const TextBatchSharedPtr &textBatch : _textLayout->getTextBatchesForRendering()) {
        for (size_t textBufferIndex = 0; textBufferIndex < textBatch->getTextBufferCount();
             textBufferIndex++) {
            textBatch->useTexture(s_texturesUniform->getId(), TEXTURE_ARRAY_BLOCK);
            textBatch->useTextBuffer(textBufferIndex, TEXT_BUFFER_BINDING,
                                     GLYPH_TRANSFORM_BUFFER_BINDING);
        }
        for (const TextBufferUniquePtr &textBuffer : textBatch->getTextBuffers()) {
            s_shaderProgram->use();
            s_vertexAssembly->use();
            textures->use(s_texturesUniform->getId(), TEXTURE_ARRAY_BLOCK);
            textBuffer->use(TEXT_BUFFER_BINDING, GLYPH_TRANSFORM_BUFFER_BINDING);
            setUniforms();
            s_charFromUniformUniform->set(glyphBuffer->getRange().first);
            const uint32_t instanceCount = static_cast<uint32_t>(textBuffer->getCurrentLength());
            rendell::setDrawType(rendell::DrawMode::ArraysInstanced,
                                 rendell::PrimitiveTopology::TriangleStrip, instanceCount);
            rendell::submit();
        }
    }
}

bool TextRenderer::init() {
    if (!s_initialized) {
        s_initialized = initStaticRendererStuff();
    }

    if (!s_initialized) {
        return false;
    }

    return s_initialized;
}

void TextRenderer::setUniforms() {
    const glm::ivec2 fontSize = _textLayout->getFontSize();

    s_matrixUniform->set(glm::value_ptr(_matrix));
    s_fontSizeUniform->set(static_cast<float>(fontSize.x), static_cast<float>(fontSize.y));
    s_textColorUniform->set(_color.r, _color.g, _color.b, _color.a);
    s_backgroundColorUniform->set(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b,
                                  _backgroundColor.a);
}

} // namespace rendell_text
