#pragma once
#include <memory>
#include <filesystem>
#include "FontRasterizationResult.h"

namespace rendell_text
{
	class IFontRaster
	{
	protected:
		IFontRaster() = default;

	public:
		virtual ~IFontRaster() = default;

	public:
		virtual bool isInitialized() const = 0;
		virtual const std::filesystem::path& getFontPath() const = 0;

		virtual bool loadFont(const std::filesystem::path& fontPath) = 0;

		virtual bool rasterize(wchar_t from, wchar_t to, uint32_t width, uint32_t height, FontRasterizationResult& result) = 0;
	};

	typedef std::shared_ptr<IFontRaster> FontRasterSharedPtr;
}
