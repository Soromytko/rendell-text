#pragma once
#include <memory>
#include <string>
#include <map>
#include "GlyphBuffer.h"
#include "IFontRaster.h"
#include "defines.h"

namespace rendell_text
{
	class RasteredFontStorage
	{
	public:
		RasteredFontStorage(IFontRasterSharedPtr fontRaster, wchar_t charRangeSize);
		~RasteredFontStorage() = default;

		void clearCache();
		void setFontSize(uint32_t width, uint32_t height);
		GlyphBufferSharedPtr rasterizeGlyphRange(wchar_t rangeIndex);

		wchar_t getRangeIndex(wchar_t character) const;
		uint32_t getFontWidth() const;
		uint32_t getFontHeight() const;
		const IFontRasterSharedPtr getFontRaster() const;

	private:
		GlyphBufferSharedPtr createGlyphBuffer(wchar_t rangeIndex);

		IFontRasterSharedPtr _fontRaster;
		uint32_t _fontWidth = 64, _fontHeight = 64;
		const wchar_t _charRangeSize;
		std::map<wchar_t, std::shared_ptr<GlyphBuffer>> _cachedGlyphBuffers{};
	};

	DECLARE_SHARED_PTR_FACTORY(RasteredFontStorage)
}
