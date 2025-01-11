#include "TextRenderer.h"

namespace rendell_text
{
	TextRendererSharedPtr createTextRenderer()
	{
		return std::make_shared<TextRenderer>();
	}
}