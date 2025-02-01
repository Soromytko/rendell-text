#pragma once
#include <map>
#include <unordered_set>
#include <rendell/rendell.h>
#include <glm/glm.hpp>
#include "TextBatch.h"
#include "RasteredFontStorage.h"

namespace rendell_text
{
	class TextRenderer
	{
	public:
		TextRenderer();
		~TextRenderer();

		bool isInitialized() const;

		void setFontPath(const std::filesystem::path& fontPath);
		void setText(const std::wstring& value);
		void setText(std::wstring&& value);
		void setMatrix(const glm::mat4& matrix);
		void setFontSize(const glm::ivec2& fontSize);
		void setColor(const glm::vec4& color);
		void setBackgroundColor(const glm::vec4 backgroundColor);

		const std::filesystem::path& getFontPath() const;
		glm::ivec2 getFontSize() const;
		const std::wstring& getText() const;
		const glm::vec4& getColor() const;
		const std::vector<uint32_t>& getTextAdvance();

		void draw();

	private:
		bool init();

		void updateShaderBuffers();

		void beginDrawing();
		void endDrawing();

		inline void updateBuffersIfNeeded();

		RasteredFontStorageSharedPtr getRasteredFontStorage(const std::filesystem::path& fontPath);
		TextBatch* createTextBatch(wchar_t character);

		bool _shouldBuffersBeUpdated = false;
		bool _shouldTextBatchBeUpdated = false;
		glm::mat4 _matrix{};
		glm::ivec2 _fontSize = glm::ivec2(64, 64);
		glm::vec4 _color{};
		glm::vec4 _backgroundColor{};

		RasteredFontStorageSharedPtr _rasteredFontStorage{ nullptr };

		std::map<uint32_t, std::unique_ptr<TextBatch>> _textBatches{};
		std::unordered_set<TextBatch*> _textBatchesForRendering{};

		std::wstring _text{};
		std::vector<uint32_t> _textAdvance{};

	};

	typedef std::shared_ptr<TextRenderer> TextRendererSharedPtr;
}
