#pragma once
#include <map>
#include <filesystem>
#include <rendell_text/private/RasteredFontStorage.h>

namespace rendell_text
{
	struct RasteredFontStoragePreset
	{
		std::filesystem::path fontPath{};
		uint32_t fontWidth{};
		uint32_t fontHeight{};
		wchar_t charRangeSize{};
	};

	class RasteredFontStorageManager
	{
	public:
		RasteredFontStorageManager() = default;
		~RasteredFontStorageManager() = default;

		void clearUnusedCache();

		RasteredFontStorageSharedPtr getRasteredFontStorage(const RasteredFontStoragePreset& preset);

	private:
		size_t hashFontPreset(const RasteredFontStoragePreset& preset) const;

		std::map<size_t, RasteredFontStorageSharedPtr> _rasteredFontStorages{};
	};
}
