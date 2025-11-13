#include "FontRaster.h"

#include <logging.h>

#include <msdfgen-ext.h>
#include <msdfgen.h>

#include <algorithm>
#include <cassert>
#include <memory>

namespace rendell_text {
FontRaster::~FontRaster() {
    if (_freetypeHandle) {
        msdfgen::deinitializeFreetype(_freetypeHandle);
        _freetypeHandle = nullptr;
    }

    if (_fontHandle) {
        msdfgen::destroyFont(_fontHandle);
        _fontHandle = nullptr;
    }
}

FontRaster::FontRaster() {
    FT_Error error = FT_Init_FreeType(&_freetype);
    if (error) {
        RT_CRITICAL("FreeType initialization failure!");
        return;
    }

    _freetypeHandle = msdfgen::initializeFreetype();
    if (!_freetypeHandle) {
        RT_CRITICAL("FreeType initialization failure using msdfgen!");
        return;
    }
}

const std::filesystem::path &FontRaster::getFontPath() const {
    return _fontPath;
}

uint32_t FontRaster::getWidth() const {
    return _width;
}

uint32_t FontRaster::getHeight() const {
    return _height;
}

int FontRaster::getFontHeight() const {
    assert(_face);
    const FT_Pos lineHeight = _face->size->metrics.height >> 6;
    return lineHeight;
}

int FontRaster::getAscender() const {
    const FT_Pos ascender = _face->size->metrics.ascender >> 6;
    return ascender;
}

int FontRaster::getDescender() const {
    const FT_Pos descender = _face->size->metrics.descender >> 6;
    return descender;
}

bool FontRaster::setFontData(const std::byte *data, size_t size) {
    assert(data);
    assert(size > 0);

    _fontHandle = msdfgen::loadFontData(
        _freetypeHandle, reinterpret_cast<const msdfgen::byte *>(data), static_cast<int>(size));
    if (!_fontHandle) {
        RT_ERROR("Failed to load msdfgen font!");
        return false;
    }

    FT_Error error = FT_New_Memory_Face(_freetype, reinterpret_cast<const FT_Byte *>(data),
                                        static_cast<FT_Long>(size), 0, &_face);
    if (error) {
        RT_ERROR("Failed to load font!");
        return false;
    }
    return true;
}

void FontRaster::setFontSize(uint32_t width, uint32_t height) {
    _width = width;
    _height = height;
}

bool FontRaster::rasterizeGlyph(Codepoint character, AtlasType atlasType, GlyphBitmap &result) {
    assert(_face);
    switch (atlasType) {
    case AtlasType::bitmap:
        return rasterizeGlyphBitmap(character, result);
    case AtlasType::sdf:
        return rasterizeGlyphSDF(character, result);
    case AtlasType::msdf:
        return rasterizeGlyphMSDF(character, result);
    case AtlasType::mtsdf:
        return rasterizeGlyphMTSDF(character, result);
    }

    return false;
}

static std::vector<std::byte> convertBitmapToVector(FT_BitmapGlyph bitmap) {
    if (bitmap->bitmap.width == 0 || bitmap->bitmap.rows == 0 || !bitmap->bitmap.buffer) {
        return {};
    }

    const size_t width = static_cast<size_t>(bitmap->bitmap.width);
    const size_t rows = static_cast<size_t>(bitmap->bitmap.rows);
    const std::byte *data = reinterpret_cast<const std::byte *>(bitmap->bitmap.buffer);
    const int pitch = bitmap->bitmap.pitch;

    std::vector<std::byte> pixels;
    pixels.resize(width * rows);
    if (pitch >= 0) {
        for (size_t rowIndex = 0; rowIndex < rows; rowIndex++) {
            std::byte *out = pixels.data() + rowIndex * width;
            const std::byte *src = data + rowIndex * static_cast<size_t>(pitch);
            std::memcpy(out, src, width);
        }
    } else {
        for (size_t rowIndex = 0; rowIndex < rows; rowIndex++) {
            std::byte *out = pixels.data() + rowIndex * width;
            const std::byte *src = data + (rows - 1 - rowIndex) * (-pitch);
            std::memcpy(out, src, width);
        }
    }
    return pixels;
}

bool FontRaster::rasterizeGlyphBitmap(Codepoint character, GlyphBitmap &result) {
    assert(_face);

    if (FT_Load_Char(_face, static_cast<FT_ULong>(character), FT_LOAD_RENDER)) {
        RT_ERROR("Failed to load Glyph {}", static_cast<char>(character));
        return false;
    }

    FT_Glyph glyph;
    if (FT_Get_Glyph(_face->glyph, &glyph)) {
        RT_ERROR("Failed to get Glyph {}", static_cast<char>(character));
        return false;
    }

    const FT_BitmapGlyph ftBitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

    result.character = character;
    result.glyphSize = glm::ivec2(ftBitmapGlyph->bitmap.width, ftBitmapGlyph->bitmap.rows);
    result.glyphBearing = glm::ivec2(ftBitmapGlyph->left, ftBitmapGlyph->top);
    result.glyphAdvance = static_cast<uint32_t>(_face->glyph->advance.x);
    result.atlasType = AtlasType::bitmap;
    result.pixels = convertBitmapToVector(ftBitmapGlyph);

    FT_Done_Glyph(glyph);

    return true;
}

bool FontRaster::rasterizeGlyphSDF(Codepoint character, GlyphBitmap &result) {
    return false;
}

bool FontRaster::rasterizeGlyphMSDF(Codepoint character, GlyphBitmap &result) {
    assert(_face);
    assert(_freetype);
    assert(_freetypeHandle);

    msdfgen::Shape shape;
    if (!msdfgen::loadGlyph(shape, _fontHandle, static_cast<msdfgen::unicode_t>(character))) {
        RT_ERROR(L"Failed to load Glyph using msdfgen: {}", static_cast<wchar_t>(character));
        return false;
    }
    shape.normalize();
    msdfgen::edgeColoringSimple(shape, 3.0);

    const auto glyphWidth = _width;
    const auto glyphHeight = _height;

    double xMin, xMax, yMin, yMax;
    shape.bound(xMin, yMin, xMax, yMax);
    const msdfgen::Vector2 scale(glyphWidth / (xMax - xMin), glyphHeight / (yMax - yMin));
    const msdfgen::Vector2 translate = -msdfgen::Vector2(xMin, yMin);
    const msdfgen::Projection projection(scale, translate);
    const msdfgen::Range range(4.0f);
    const msdfgen::SDFTransformation transform(projection, range);

    msdfgen::Bitmap<float, 3> msdf(static_cast<int>(glyphWidth), static_cast<int>(glyphHeight));
    msdfgen::generateMSDF(msdf, shape, transform);

    if (FT_Error error = FT_Load_Char(_face, static_cast<FT_ULong>(character), FT_LOAD_DEFAULT)) {
        RT_ERROR(L"Failed to load Glyph: {}", static_cast<wchar_t>(character));
        return false;
    }
    FT_GlyphSlot slot = _face->glyph;

    const size_t floatCount = glyphWidth * glyphHeight * 3;

    result.character = character;
    result.glyphSize = glm::ivec2(_width, _height);
    result.glyphBearing = glm::ivec2(static_cast<int>(slot->metrics.horiBearingX >> 6),
                                     static_cast<int>(slot->metrics.horiBearingY >> 6));
    result.glyphAdvance = static_cast<uint32_t>(slot->metrics.horiAdvance >> 6);
    result.atlasType = AtlasType::msdf;
    result.pixels.resize(_width * _height * 3 * sizeof(float));
    std::memcpy(result.pixels.data(), static_cast<const float *>(msdf), result.pixels.size());

    return true;
}

bool FontRaster::rasterizeGlyphMTSDF(Codepoint character, GlyphBitmap &result) {
    return false;
}

bool FontRaster::rasterizeChar(Codepoint character, FT_Glyph &result) {
    if (FT_Load_Char(_face, static_cast<FT_ULong>(character), FT_LOAD_RENDER)) {
        RT_ERROR("Failed to load Glyph {}", static_cast<char>(character));
        return false;
    }

    if (FT_Get_Glyph(_face->glyph, &result)) {
        RT_ERROR("Failed to get Glyph {}", static_cast<char>(character));
        return false;
    }

    return true;
}

FT_Glyph FontRaster::rasterizeGlyphStub() {
    FT_Glyph result;
    rasterizeChar(0, result);
    return result;
}
} // namespace rendell_text
