#include <GlyphRaster.h>

#include <FontLibrary.h>
#include <SharedFontData.h>
#include <logging.h>

#include <msdfgen-ext.h>
#include <msdfgen.h>

#include <array>
#include <cassert>

namespace rendell_text {
RasterizeResult GlyphRaster::rasterize(std::span<const GlyphId> glyphs, FontInstance fontInstance,
                                       AtlasType atlasType) {
    const IFontData *fontData;
    const auto lock = FontLibrary::getInstance()->readAndLock(fontInstance.fontHandle, fontData);
    assert(dynamic_cast<const SharedFontData *>(fontData) != nullptr);
    const auto sharedFontData = static_cast<const SharedFontData *>(fontData);

    RasterizeResult result;
    if (atlasType == AtlasType::msdf) {
        rasterizeMSDF(glyphs, fontInstance.size, sharedFontData->msdf, result.rasterisedGlyphs);
    } else {
        assert(false);
    }
    return result;
}

RasterizedGlyph GlyphRaster::rasterize(GlyphId glyphId, FontInstance fontInstance,
                                       AtlasType atlasType) {

    const IFontData *fontData;
    const auto lock = FontLibrary::getInstance()->readAndLock(fontInstance.fontHandle, fontData);
    assert(dynamic_cast<const SharedFontData *>(fontData) != nullptr);
    const auto sharedFontData = static_cast<const SharedFontData *>(fontData);

    std::array<GlyphId, 1> glyphs = {glyphId};
    RasterizedGlyphList rasterisedGlyphs;
    if (atlasType == AtlasType::msdf) {
        rasterizeMSDF(glyphs, fontInstance.size, sharedFontData->msdf, rasterisedGlyphs);
    } else {
        assert(false);
    }
    assert(rasterisedGlyphs.size() == 1);
    return rasterisedGlyphs[0];
}

bool GlyphRaster::rasterizeMSDF(std::span<const GlyphId> glyphs, Size fontSize,
                                const MSDF_Resource &msdf, RasterizedGlyphList &result) const {
    result.clear();
    result.reserve(glyphs.size());

    assert(msdf.fontHandle);
    const auto fontHandle = msdf.fontHandle.get();

    msdfgen::Shape shape;

    const double range = 4.0;
    const double unitsPerEm = static_cast<double>(msdf.metrics.emSize);
    const double scale = static_cast<double>(fontSize.height) / unitsPerEm;
    const msdfgen::Vector2 msdfScale(scale, scale);
    const msdfgen::Vector2 frame(fontSize.width, fontSize.height);

    for (const GlyphId glyphId : glyphs) {
        shape.contours.clear();
        double advance;
        if (!msdfgen::loadGlyph(shape, fontHandle, static_cast<msdfgen::GlyphIndex>(glyphId),
                                &advance)) {
            RT_ERROR(L"Failed to load GlyphId[{}] using msdfgen", static_cast<size_t>(glyphId));
            return false;
        }
        shape.normalize();
        msdfgen::edgeColoringSimple(shape, 3.0);

        double xMin, xMax, yMin, yMax;
        shape.bound(xMin, yMin, xMax, yMax);
        const msdfgen::Projection projection(msdfgen::Vector2(scale, scale),
                                             msdfgen::Vector2(-xMin, -yMin) + (range / scale));
        const Size bitmapSize = Size{
            .width = static_cast<Size::Type>((xMax - xMin) * scale + range * 2.0 + 0.5),
            .height = static_cast<Size::Type>((yMax - yMin) * scale + range * 2.0 + 0.5),
        };
        const size_t pixelsSize = sizeof(float) * bitmapSize.width * bitmapSize.height * 3;
        if (pixelsSize == 0) {
            assert(false); ////////TODO
            continue;
        }
        std::vector<std::byte> pixelBuffer(pixelsSize);
        msdfgen::BitmapRef<float, 3> msdf(reinterpret_cast<float *>(pixelBuffer.data()),
                                          static_cast<int>(bitmapSize.width),
                                          static_cast<int>(bitmapSize.height));
        msdfgen::generateMSDF(msdf, shape, projection, range);

        result.push_back(RasterizedGlyph{
            .id = glyphId,
            .bearingX = static_cast<float>(xMin * scale - range),
            .bearingY = static_cast<float>(yMax * scale + range),
            .advance = static_cast<float>(advance * scale),
            .atlasType = AtlasType::msdf,
            .bitmap =
                GlyphBitmap{
                    .size = bitmapSize,
                    .pixels = std::move(pixelBuffer),
                },
        });
    }

    return !result.empty();
}

} // namespace rendell_text