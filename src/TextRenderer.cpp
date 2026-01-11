#include <TextRenderer.h>

#include <rendell/oop/rendell_oop.h>
#include <rendell_text/IGlyphAtlasTexture.h>
#include <rendell_text/ITextBuffer.h>

#include "res_Shaders_TextRenderer_fs.h"
#include "res_Shaders_TextRenderer_vs.h"
#include <logging.h>

#include <cassert>
#include <glm/gtc/type_ptr.hpp>

#define TEXTURE_ARRAY_BLOCK 0
#define TEXT_BUFFER_BINDING 0
#define GLYPH_TRANSFORM_BUFFER_BINDING 1
#define GLYPH_UV_BUFFER_BINDING 1

namespace rendell_text {
struct BasicRenderResources final {
    rendell::oop::VertexAssembly vertexAssembly;
    rendell::oop::ShaderProgram shaderProgram;
    rendell::oop::Mat4Uniform matrixUniform;
    rendell::oop::Float4Uniform textColorUniform;
    rendell::oop::Float4Uniform backgroundColorUniform;
    rendell::oop::Int1Uniform charFromUniformUniform;
    rendell::oop::Sampler2DUniform texturesUniform;
};

static std::unique_ptr<BasicRenderResources> s_basicRenderResources{nullptr};

static rendell::oop::VertexAssembly createVertexAssembly() {
    static const std::vector<float> vertexPos{
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    };
    static const std::vector<uint32_t> indices{0, 0, 0, 0};

    auto indexBuffer = rendell::oop::makeIndexBuffer(indices.data(), indices.size());
    auto vertexBuffer = rendell::oop::makeVertexBuffer(vertexPos.data(), vertexPos.size());
    auto vertexLayout =
        rendell::VertexLayout().addAttribute(0, rendell::ShaderDataType::float2, false, 0);
    return rendell::oop::VertexAssembly(indexBuffer, std::vector{vertexBuffer},
                                        std::vector{vertexLayout});
}

static rendell::oop::ShaderProgram createShaderProgram(const std::string &vertexSrc,
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

    auto program = rendell::oop::ShaderProgram(
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

bool TextRenderer::initBasicRenderResources() {
    std::string vertexSrc, fragmentSrc;
    if (!loadShaders(vertexSrc, fragmentSrc)) {
        RT_ERROR("Shader loading failed");
        return false;
    }

    s_basicRenderResources = std::make_unique<BasicRenderResources>(BasicRenderResources{
        .vertexAssembly = createVertexAssembly(),
        .shaderProgram = createShaderProgram(vertexSrc, fragmentSrc),
        .matrixUniform = rendell::oop::Mat4Uniform("u_Matrix"),
        .textColorUniform = rendell::oop::Float4Uniform("u_TextColor"),
        .backgroundColorUniform = rendell::oop::Float4Uniform("u_BackgroundColor"),
        .charFromUniformUniform = rendell::oop::Int1Uniform("u_CharFrom"),
        .texturesUniform = rendell::oop::Sampler2DUniform("u_Textures"),
    });

    return true;
}

void TextRenderer::releaseBasicRenderResources() {
    s_basicRenderResources.reset();
}

TextRenderer::TextRenderer(std::shared_ptr<ITextBuffer> textBuffer,
                           std::shared_ptr<IGlyphAtlasTexture> atlasTexture) {
    assert(s_basicRenderResources);
    assert(textBuffer);
    assert(atlasTexture);
}

std::shared_ptr<ITextBuffer> TextRenderer::getTextBuffer() const {
    assert(_textBuffer);
    return _textBuffer;
}

std::shared_ptr<IGlyphAtlasTexture> TextRenderer::getGlyphAtlasTexture() const {
    assert(_atlasTexture);
    return _atlasTexture;
}

const glm::vec4 &TextRenderer::getColor() const {
    return _color;
}

void TextRenderer::setTextBuffer(std::shared_ptr<ITextBuffer> textBuffer) {
    assert(textBuffer);
    _textBuffer = textBuffer;
}

void TextRenderer::setGlyphAtlasTexture(std::shared_ptr<IGlyphAtlasTexture> atlasTexture) {
    assert(atlasTexture);
    _atlasTexture = atlasTexture;
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
    assert(_textBuffer);
    assert(_atlasTexture);

    _textBuffer->prepare();
    _atlasTexture->prepare();
}

void TextRenderer::draw() {
    assert(s_basicRenderResources);
    assert(_atlasTexture);
    assert(_textBuffer);

    prepare();

    if (_textBuffer->isEmtpy()) {
        return;
    }

    s_basicRenderResources->shaderProgram.use();
    s_basicRenderResources->vertexAssembly.use();
    _textBuffer->use(GLYPH_TRANSFORM_BUFFER_BINDING, GLYPH_UV_BUFFER_BINDING);
    _atlasTexture->use(s_basicRenderResources->texturesUniform.getId(), TEXTURE_ARRAY_BLOCK);

    s_basicRenderResources->matrixUniform.set(glm::value_ptr(_matrix));
    s_basicRenderResources->textColorUniform.set(_color.r, _color.g, _color.b, _color.a);
    s_basicRenderResources->backgroundColorUniform.set(_backgroundColor.r, _backgroundColor.g,
                                                       _backgroundColor.b, _backgroundColor.a);

    rendell::setDrawType(rendell::DrawMode::ArraysInstanced,
                         rendell::PrimitiveTopology::TriangleStrip,
                         static_cast<uint32_t>(_textBuffer->getLength()));
    rendell::submit();
}

} // namespace rendell_text
