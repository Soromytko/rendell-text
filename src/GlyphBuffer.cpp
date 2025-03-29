#include <rendell_text/private/GlyphBuffer.h>

namespace rendell_text
{
	GlyphBuffer::GlyphBuffer(wchar_t from, wchar_t to, FontRasterizationResult&& fontRasterizationResult)
	{
#ifdef _DEBIG
		assert(from > 0);
		assert(from < to);
#endif
		_range = { from, to };

		_fontRasterizationResult = std::move(fontRasterizationResult);
		_textures = _fontRasterizationResult.texture2DArray;
	}

	void GlyphBuffer::bind(uint32_t textureBlock) const
	{
		_textures->bind(textureBlock);
	}

	void GlyphBuffer::unbind() const
	{
		_textures->unbind();
	}

	const RasterizedChar& GlyphBuffer::getRasterizedChar(wchar_t character) const
	{
		const size_t index = static_cast<size_t>(character - _range.first);
#ifdef _DEBUG
		assert(index >= 0);
		assert(index < _range.second);
#endif
		return _fontRasterizationResult.rasterizedChars[index];
	}

	const std::vector<RasterizedChar>& GlyphBuffer::getRasterizedChars() const
	{
		return _fontRasterizationResult.rasterizedChars;
	}

	const std::pair<wchar_t, wchar_t>& GlyphBuffer::getRange() const
	{
		return _range;
	}
}
