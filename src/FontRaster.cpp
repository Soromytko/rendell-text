#include "FontRaster.h"

namespace rendell_text {
static uint32_t s_instanceCount = 0;
static bool s_freeTypeInitialized = false;
static FT_Library s_freetype;

FontRaster::FontRaster(const std::filesystem::path &fontPath, uint32_t width, uint32_t height) {
    s_instanceCount++;

    if (!init()) {
        RT_ERROR("Initialization failure");
        return;
    }

    if (!fontPath.empty()) {
        loadFont(fontPath, width, height);
    }
}

FontRaster::~FontRaster() {
    releaseFace();

    s_instanceCount--;
    if (s_instanceCount == 0 && s_freeTypeInitialized) {
        FT_Done_FreeType(s_freetype);
        s_freeTypeInitialized = false;
    }
}

bool FontRaster::isInitialized() const {
    return s_freeTypeInitialized && _face;
}

const std::filesystem::path &FontRaster::getFontPath() const {
    return _fontPath;
}

int FontRaster::getFontHeight() const {
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

bool FontRaster::loadFont(const std::filesystem::path &fontPath, uint32_t width, uint32_t height) {
    releaseFace();

    _fontPath = fontPath;
    _width = width;
    _height = height;
    const std::string &path = _fontPath.string();
    if (path.empty() || FT_New_Face(s_freetype, path.c_str(), 0, &_face)) {
        RT_ERROR("Failed to create font face {}", fontPath.string());
        return false;
    }
    FT_Set_Pixel_Sizes(_face, _width, _height);

    return true;
}

bool FontRaster::rasterize(wchar_t from, wchar_t to, FontRasterizationResult &result) {
#ifdef _DEBUG
    assert(s_freeTypeInitialized);
    assert(from < to);
#endif

    if (!_face) {
        RT_ERROR("Font face is missing");
        return false;
    }

    const uint32_t charCount = static_cast<uint32_t>(to - from);
    auto texture2DArray =
        rendell::oop::makeTexture2DArray(_width, _height, charCount, rendell::TextureFormat::R);
    std::vector<RasterizedChar> rasterizedChars{};
    rasterizedChars.reserve(charCount);

    for (wchar_t currentChar = from; currentChar < to; currentChar++) {
        FT_Glyph glyph;
        if (!rasterizeChar(currentChar, glyph)) {
            RT_ERROR("Failed to rasterize Glyph {}", static_cast<char>(currentChar));
            glyph = rasterizeGlyphStub();
        }

        const FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

        if (bitmapGlyph->bitmap.width > 0 && bitmapGlyph->bitmap.rows > 0) {
            texture2DArray->setSubData(
                static_cast<uint32_t>(currentChar - from),
                static_cast<uint32_t>(bitmapGlyph->bitmap.width),
                static_cast<uint32_t>(bitmapGlyph->bitmap.rows),
                reinterpret_cast<const rendell::byte_t *>(bitmapGlyph->bitmap.buffer));
        }

        RasterizedChar rasterizedChar{
            currentChar, glm::ivec2(bitmapGlyph->bitmap.width, bitmapGlyph->bitmap.rows),
            glm::ivec2(bitmapGlyph->left, bitmapGlyph->top),
            static_cast<uint32_t>(_face->glyph->advance.x)};
        rasterizedChars.push_back(std::move(rasterizedChar));

        FT_Done_Glyph(glyph);
    }

    result = {texture2DArray, std::move(rasterizedChars)};
    return true;
}

static bool initFreeType() {
    return !FT_Init_FreeType(&s_freetype);
}

bool FontRaster::init() {
    if (!s_freeTypeInitialized) {
        s_freeTypeInitialized = initFreeType();
        if (!s_freeTypeInitialized) {
            RT_ERROR("Could not init FreeType Library");
            return false;
        }
    }

    return s_freeTypeInitialized;
}

void FontRaster::releaseFace() {
    if (_face) {
        FT_Done_Face(_face);
        _face = nullptr;
    }
}

bool FontRaster::rasterizeChar(wchar_t character, FT_Glyph &result) {
    if (FT_Load_Char(_face, character, FT_LOAD_RENDER)) {
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
