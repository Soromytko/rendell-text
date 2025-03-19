#pragma once
#include <map>
#include <unordered_set>
#include <rendell/rendell.h>
#include <glm/glm.hpp>
#include "TextLayout.h"
#include "TextBatch.h"
#include "RasteredFontStorage.h"
#include "defines.h"

namespace rendell_text
{
	class TextRenderer final
	{
	public:
		TextRenderer();
		~TextRenderer();

		bool isInitialized() const;
		const TextLayoutSharedPtr& getTextLayout() const;

		void setTextLayout(const TextLayoutSharedPtr& textLayout);
		void setMatrix(const glm::mat4& matrix);
		void setColor(const glm::vec4& color);
		void setBackgroundColor(const glm::vec4 backgroundColor);

		const glm::vec4& getColor() const;

		void draw();

	private:
		bool init();
		void beginDrawing();
		void endDrawing();

		TextLayoutSharedPtr _textLayout{};
		glm::mat4 _matrix{};
		glm::vec4 _color{};
		glm::vec4 _backgroundColor{};

	};

	DECLARE_SHARED_PTR_FACTORY(TextRenderer)
}
