#pragma once
#include <map>
#include <glm/glm.hpp>
#include <rendell/rendell.h>

namespace rendell_text
{
	struct RasterizedChar
	{
		wchar_t character{};
		glm::ivec2 glyphSize{};
		glm::ivec2 glyphBearing{};
		uint32_t glyphAdvance{};
	};

	struct FontRasterizationResult
	{
		rendell::Texture2DArraySharedPtr texture2DArray{};
		std::vector<RasterizedChar> rasterizedChars{};
	};
}
