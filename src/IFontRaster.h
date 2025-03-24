#pragma once
#include <memory>
#include <filesystem>
#include "FontRasterizationResult.h"
#include "defines.h"

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
		virtual int getFontHeight() const = 0;
		virtual int getAscender() const = 0;
		virtual int getDescender() const = 0;

		virtual bool loadFont(const std::filesystem::path& fontPath, uint32_t width, uint32_t height) = 0;

		virtual bool rasterize(wchar_t from, wchar_t to, FontRasterizationResult& result) = 0;
	};

	DECLARE_SHARED_PTR(IFontRaster)
}
