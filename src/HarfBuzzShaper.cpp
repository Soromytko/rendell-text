#include <HarfBuzzShaper.h>

#include <FontLibrary.h>
#include <SharedFontData.h>

#include <hb.h>

#include <cassert>
#include <memory>

namespace rendell_text {
HarfBuzzShaper::HarfBuzzShaper(ShaperConfig config)
    : _features({
          {HB_TAG('k', 'e', 'r', 'n'), config.enableKerning ? 1u : 0u, 0, (unsigned int)-1},
          {HB_TAG('l', 'i', 'g', 'a'), config.enableLigatures ? 1u : 0u, 0, (unsigned int)-1},
      }) {
}

ShapeResult HarfBuzzShaper::shape(const TextRun &textRun) {
    ShapeResult result{
        .fontInstance = textRun.fontInstance,
        .shapedGlyphs = ShapedGlyphList(textRun.text.length()),
    };
    shape(textRun, result);
    return result;
}

void HarfBuzzShaper::shape(const TextRun &textRun, ShapeResult &result) {
    const IFontData *fontData;
    auto lock = FontLibrary::getInstance()->readAndLock({}, fontData);
    assert(dynamic_cast<const SharedFontData *>(fontData) != nullptr);
    const auto sharedFontData = static_cast<const SharedFontData *>(fontData);

    doShape(sharedFontData->hb, textRun, result);
}

static void add_text_to_buffer(hb_buffer_t *buf, const std::u32string &text) {
    hb_buffer_add_utf32(buf, reinterpret_cast<const uint32_t *>(text.data()),
                        static_cast<int>(text.length()), 0, -1);
}

static void add_text_to_buffer(hb_buffer_t *buf, const std::string &text) {
    hb_buffer_add_utf8(buf, text.data(), static_cast<int>(text.length()), 0, -1);
}

struct HB_Buffer_Deleter {
    void operator()(hb_buffer_t *ptr) {
        if (ptr) {
            hb_buffer_destroy(ptr);
        }
    }
};

bool HarfBuzzShaper::doShape(const HB_Resource &hb, const TextRun &textRun,
                             ShapeResult &result) const {
    assert(hb.face);

    result.shapedGlyphs.clear();
    result.fontInstance = textRun.fontInstance;

    std::unique_ptr<hb_buffer_t, HB_Buffer_Deleter> localBuf(hb_buffer_create());
    auto buf = localBuf.get();

    unsigned int glyphCoufnt = hb_face_get_glyph_count(hb.face.get());

    hb_font_set_scale(hb.font.get(), static_cast<int>(textRun.fontInstance.size.width * 64.0f),
                      static_cast<int>(textRun.fontInstance.size.height * 64.0f));

    add_text_to_buffer(buf, textRun.text);
    hb_buffer_guess_segment_properties(buf);
    hb_shape(hb.font.get(), buf, _features.data(), static_cast<unsigned int>(_features.size()));

    unsigned int glyphCount;
    hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buf, &glyphCount);
    hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buf, &glyphCount);

    result.shapedGlyphs.reserve(static_cast<size_t>(glyphCount));

    for (size_t i = 0; i < glyphCount; ++i) {
        result.shapedGlyphs.push_back(ShapedGlyph{
            .id = static_cast<decltype(ShapedGlyph::id)>(glyphInfo[i].codepoint),
            .advanceX = static_cast<decltype(ShapedGlyph::advanceX)>(glyphPos[i].x_advance / 64.0f),
            .advanceY = static_cast<decltype(ShapedGlyph::advanceY)>(glyphPos[i].y_advance / 64.0f),
            .offsetX = static_cast<decltype(ShapedGlyph::offsetX)>(glyphPos[i].x_offset / 64.0f),
            .offsetY = static_cast<decltype(ShapedGlyph::offsetY)>(glyphPos[i].y_offset / 64.0f),
            .cluster = static_cast<decltype(ShapedGlyph::cluster)>(glyphInfo[i].cluster),
        });
    }

    return !result.shapedGlyphs.empty();
}

} // namespace rendell_text