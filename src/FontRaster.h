#pragma once
#include <iostream>
#include "IFontRaster.h"
#include "freetype.h"

namespace rendell_text
{
	class FontRaster : public IFontRaster
	{
	public:
		FontRaster(const std::filesystem::path& fontPath = "");
		~FontRaster();

		bool isInitialized() const override;
		const std::filesystem::path& getFontPath() const override;
		GeneralFontMetrices getGeneralFontMetrices() const override;

		bool loadFont(const std::filesystem::path& fontPath) override;

		bool rasterize(wchar_t from, wchar_t to, uint32_t width, uint32_t height, FontRasterizationResult& result) override;

	private:
		bool init();
		void releaseFace();
		bool rasterizeChar(wchar_t character, FT_Glyph& result);
		FT_Glyph rasterizeGlyphStub();

		FT_Face _face{ nullptr };
		std::filesystem::path _fontPath{};
	};
}
