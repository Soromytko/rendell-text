#include "FontRaster.h"

namespace rendell_text
{
	static uint32_t s_instanceCount = 0;
	static bool s_freeTypeInitialized = false;
	static FT_Library s_freetype;

	FontRaster::FontRaster(const std::filesystem::path& fontPath)
	{
		s_instanceCount++;

		if (!init())
		{
			std::cout << "ERROR::FontRaster: Initialization failure" << std::endl;
			return;
		}

		if (!fontPath.empty())
		{
			loadFont(fontPath);
		}
	}

	FontRaster::~FontRaster()
	{
		releaseFace();

		s_instanceCount--;
		if (s_instanceCount == 0 && s_freeTypeInitialized)
		{
			FT_Done_FreeType(s_freetype);
			s_freeTypeInitialized = false;
		}
	}

	bool FontRaster::isInitialized() const
	{
		return s_freeTypeInitialized && _face;
	}

	const std::filesystem::path& FontRaster::getFontPath() const
	{
		return _fontPath;
	}

	GeneralFontMetrices FontRaster::getGeneralFontMetrices() const
	{
		FT_Pos lineHeight = _face->size->metrics.height >> 6;
		FT_Pos ascender = _face->size->metrics.ascender >> 6;
		FT_Pos descender = _face->size->metrics.descender >> 6;

		GeneralFontMetrices result;
		result.height = static_cast<int32_t>(lineHeight);
		result.ascender = static_cast<int32_t>(ascender);
		result.descender = static_cast<int32_t>(descender);
		return result;
	}

	bool FontRaster::loadFont(const std::filesystem::path& fontPath)
	{
		releaseFace();

		_fontPath = fontPath;
		const std::string& path = _fontPath.string();
		if (path.empty() || FT_New_Face(s_freetype, path.c_str(), 0, &_face)) {
			std::cout << "ERROR::FREETYPE: Failed to create font face " << fontPath << std::endl;
			return false;
		}

		return true;
	}

	bool FontRaster::rasterize(
		wchar_t from, wchar_t to,
		uint32_t width, uint32_t height,
		FontRasterizationResult& result)
	{
#ifdef _DEBUG
		assert(s_freeTypeInitialized);
		assert(from < to);
#endif

		if (!_face)
		{
			std::cout << "ERROR::FREETYPE: Font face is missing" << std::endl;
			return false;
		}

		FT_Set_Pixel_Sizes(_face, width, height);

		const size_t charCount = static_cast<size_t>(to - from);
		auto texture2DArray = rendell::createTexture2DArray(width, height, charCount, rendell::TextureFormat::R);
		std::vector<RasterizedChar> rasterizedChars{};
		rasterizedChars.reserve(charCount);

		for (wchar_t currentChar = from; currentChar < to; currentChar++)
		{
			FT_Glyph glyph;
			if (!rasterizeChar(currentChar, glyph))
			{
				std::cout << "ERROR::FREETYTPE: Failed to rasterize Glyph " << currentChar << std::endl;
				glyph = rasterizeGlyphStub();
			}

			const FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

			texture2DArray->setSubTextureData(
				static_cast<uint32_t>(currentChar - from),
				static_cast<uint32_t>(bitmapGlyph->bitmap.width),
				static_cast<uint32_t>(bitmapGlyph->bitmap.rows),
				static_cast<const uint8_t*>(bitmapGlyph->bitmap.buffer)
			);

			RasterizedChar rasterizedChar{
				currentChar,
				glm::ivec2(bitmapGlyph->bitmap.width, bitmapGlyph->bitmap.rows),
				glm::ivec2(bitmapGlyph->left, bitmapGlyph->top),
				static_cast<uint32_t>(_face->glyph->advance.x)
			};
			rasterizedChars.push_back(std::move(rasterizedChar));

			FT_Done_Glyph(glyph);
		}

		result = { texture2DArray, std::move(rasterizedChars) };
		return true;
	}

	static bool initFreeType()
	{
		return !FT_Init_FreeType(&s_freetype);
	}

	bool FontRaster::init()
	{
		if (!s_freeTypeInitialized)
		{
			s_freeTypeInitialized = initFreeType();
			if (!s_freeTypeInitialized)
			{
				std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
				return false;
			}
		}

		return s_freeTypeInitialized;
	}

	void FontRaster::releaseFace()
	{
		if (_face)
		{
			FT_Done_Face(_face);
			_face = nullptr;
		}
	}

	bool FontRaster::rasterizeChar(wchar_t character, FT_Glyph& result)
	{
		if (FT_Load_Char(_face, character, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYPE: Failed to load Glyph " << character << std::endl;
			return false;
		}

		if (FT_Get_Glyph(_face->glyph, &result))
		{
			std::cout << "ERROR::FREETYPE: Failed to get Glyph " << character << std::endl;
			return false;
		}

		return true;
	}

	FT_Glyph FontRaster::rasterizeGlyphStub()
	{
		FT_Glyph result;
		rasterizeChar(0, result);
		return result;
	}
}
