#include "RasteredFontStorage.h"

namespace rendell_text
{
	RasteredFontStorage::RasteredFontStorage(FontRasterSharedPtr fontRaster, wchar_t charRangeSize) :
		_fontRaster(fontRaster), _charRangeSize(charRangeSize)
	{

	}

	void RasteredFontStorage::clearCache()
	{
		_cachedGlyphBuffers.clear();
	}

	void RasteredFontStorage::setFontSize(uint32_t width, uint32_t height)
	{
		_fontWidth = width;
		_fontHeight = height;
		clearCache();
	}

	GlyphBufferSharedPtr RasteredFontStorage::rasterizeGlyphRange(wchar_t rangeIndex)
	{
		if (auto it = _cachedGlyphBuffers.find(rangeIndex); it != _cachedGlyphBuffers.end())
		{
			return it->second;
		}

		GlyphBufferSharedPtr glyphBufferPtr = createGlyphBuffer(rangeIndex);
		_cachedGlyphBuffers[rangeIndex] = glyphBufferPtr;
		return glyphBufferPtr;
	}

	wchar_t RasteredFontStorage::getRangeIndex(wchar_t character) const
	{
		return character / _charRangeSize;
	}

	uint32_t RasteredFontStorage::getFontWidth() const
	{
		return _fontWidth;
	}

	uint32_t RasteredFontStorage::getFontHeight() const
	{
		return _fontHeight;
	}

	const FontRasterSharedPtr RasteredFontStorage::getFontRaster() const
	{
		return _fontRaster;
	}

	GlyphBufferSharedPtr RasteredFontStorage::createGlyphBuffer(wchar_t rangeIndex)
	{
		const wchar_t from = rangeIndex * _charRangeSize;
		const wchar_t to = (rangeIndex + 1) * _charRangeSize;
		FontRasterizationResult fontRasterizationResult;
		if (!_fontRaster->rasterize(from, to, _fontWidth, _fontHeight, fontRasterizationResult))
		{
			std::cout << "ERROR::RasteredFontStorage: Rasterization failure, {" << from << ", " << to << "}" << std::endl;
			return nullptr;
		}

		return std::make_shared<GlyphBuffer>(from, to, std::move(fontRasterizationResult));
	}
}
