#pragma once
#include <memory>
#include <rendell/rendell.h>
#include "IFontRaster.h"

class GlyphBuffer
{
public:
	GlyphBuffer(wchar_t from, wchar_t to, FontRasterizationResult&& fontRasterizationResult);

	void bind(uint32_t textureBlock) const;
	void unbind() const;

	const RasterizedChar& getRasterizedChar(wchar_t character) const;
	const std::vector<RasterizedChar>& getRasterizedChars() const;
	const std::pair<wchar_t, wchar_t>& getRange() const;

private:
	FontRasterizationResult _fontRasterizationResult{};

	std::pair<wchar_t, wchar_t> _range{};
	std::unique_ptr<rendell::Texture2DArray> _textures{};
};

typedef std::shared_ptr<GlyphBuffer> GlyphBufferSharedPtr;
