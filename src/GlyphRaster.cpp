#include <GlyphRaster.h>

#include <FontLibrary.h>
#include <SharedFontData.h>
#include <logging.h>
#include <utils/BmpSaver.h>

#include <msdfgen-ext.h>
#include <msdfgen.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <vector>

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

static void testMsdfgen() {
    if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype()) {
        if (msdfgen::FontHandle *font = msdfgen::loadFont(ft, "C:\\Windows\\Fonts\\arialbd.ttf")) {
            msdfgen::Shape shape;
            if (msdfgen::loadGlyph(shape, font, 'A', msdfgen::FONT_SCALING_EM_NORMALIZED)) {
                shape.normalize();
                edgeColoringSimple(shape, 3.0);
                msdfgen::Bitmap<float, 3> msdf(32, 32);
                msdfgen::SDFTransformation t(
                    msdfgen::Projection(32.0, msdfgen::Vector2(0.125, 0.125)),
                    msdfgen::Range(0.125));
                msdfgen::generateMSDF(msdf, shape, t);
                saveToBmp(msdf, "output_msdf.png");
            }
            msdfgen::destroyFont(font);
        }
        msdfgen::deinitializeFreetype(ft);
    }
    // exit(0);
}

RasterizedGlyph GlyphRaster::rasterize(GlyphId glyphId, FontInstance fontInstance,
                                       AtlasType atlasType) {

    // testMsdfgen();

    const IFontData *fontData;
    const auto lock = FontLibrary::getInstance()->readAndLock(fontInstance.fontHandle, fontData);
    assert(dynamic_cast<const SharedFontData *>(fontData) != nullptr);
    const auto sharedFontData = static_cast<const SharedFontData *>(fontData);

    std::array<GlyphId, 1> glyphs = {glyphId};
    RasterizedGlyphList rasterisedGlyphs;
    switch (atlasType) {
    case AtlasType::bitmap: {
        rasterizeBitmap(glyphs, fontInstance.size, sharedFontData->msdf, rasterisedGlyphs);
        break;
    }
    case AtlasType::sdf: {
        rasterizeSDF(glyphs, fontInstance.size, sharedFontData->msdf, rasterisedGlyphs);
        break;
    }
    case AtlasType::msdf: {
        rasterizeMSDF(glyphs, fontInstance.size, sharedFontData->msdf, rasterisedGlyphs);
        break;
    }
    case AtlasType::mtsdf: {
        assert(false);
    }
    }
    return !rasterisedGlyphs.empty() ? rasterisedGlyphs[0] : RasterizedGlyph{};
}

bool GlyphRaster::rasterizeBitmap(std::span<const GlyphId> glyphs, Size fontSize,
                                  const MSDF_Resource &msdf, RasterizedGlyphList &result) const {
    for (const auto glyphId : glyphs) {
        msdfgen::Shape shape;
        double advance = 0.0;
        if (msdfgen::loadGlyph(shape, msdf.fontHandle.get(), msdfgen::GlyphIndex(glyphId),
                               &advance)) {

            shape.normalize();
            msdfgen::Shape::Bounds bounds = shape.getBounds();

            int width = 0, height = 0;
            double l = 0, b = 0, r = 0, t = 0;

            if (bounds.l < bounds.r && bounds.b < bounds.t) {
                l = bounds.l;
                b = bounds.b;
                r = bounds.r;
                t = bounds.t;
                width = static_cast<int>(ceil(r - l));
                height = static_cast<int>(ceil(t - b));
            }

            RasterizedGlyph glyph{};
            glyph.id = glyphId;
            glyph.advance = static_cast<float>(advance);
            glyph.bearingX = static_cast<float>(l);
            glyph.bearingY = static_cast<float>(t); // Обычно bearingY — это верхняя точка
            glyph.atlasType = AtlasType::bitmap;    // Или ваш тип для обычного растра
            glyph.bitmap.size = Size{
                .width = static_cast<Size::Type>(width),
                .height = static_cast<Size::Type>(height),
            };

            if (width > 0 && height > 0) {
                msdfgen::Bitmap<float, 1> raster(width, height);
                msdfgen::Projection projection(msdfgen::Vector2(1.0, 1.0),
                                               msdfgen::Vector2(-l, -b));
                // msdfgen::scanlineRender(raster, shape, projection);

                glyph.bitmap.pixels.reserve(width * height);
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        float p = *raster(x, y);
                        unsigned char pixel =
                            static_cast<unsigned char>(std::clamp(p * 255.0f, 0.0f, 255.0f));
                        glyph.bitmap.pixels.push_back(static_cast<std::byte>(pixel));
                    }
                }
            }

            result.push_back(std::move(glyph));
        }
    }

    return true;
}

bool GlyphRaster::rasterizeSDF(std::span<const GlyphId> glyphs, Size fontSize,
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
        msdfgen::Bitmap<float, 3> msdf(fontSize.width, fontSize.height);
        msdfgen::Vector2 translate(0.0, 0.0);

        generateMSDF(msdf, shape, msdfgen::Projection(scale, translate), range);
        saveToBmp(msdf, "output_msdf.png");
        exit(0);

        const Size bitmapSize = Size{
            .width = static_cast<Size::Type>(fontSize.width),
            .height = static_cast<Size::Type>(fontSize.height),
        };
        const size_t pixelsSize = sizeof(float) * bitmapSize.area() * 3;
        if (pixelsSize == 0) {
            assert(false); ////////TODO
            continue;
        }

        const float *pixelsPtr = (const float *)msdf;
        size_t totalBytes = fontSize.area() * 3 * sizeof(float);
        std::vector<std::byte> byte_vector(totalBytes);
        std::memcpy(byte_vector.data(), pixelsPtr, totalBytes);

        result.push_back(RasterizedGlyph{
            .id = glyphId,
            .bearingX = 0,
            .bearingY = 0,
            .advance = static_cast<float>(advance * scale),
            .atlasType = AtlasType::msdf,
            .bitmap =
                GlyphBitmap{
                    .size = bitmapSize,
                    .pixels = byte_vector,
                },
        });
    }

    return !result.empty();
}

bool GlyphRaster::rasterizeMSDF(std::span<const GlyphId> glyphs, Size fontSize,
                                const MSDF_Resource &msdf, RasterizedGlyphList &result) const {
    result.clear();
    result.reserve(glyphs.size());

    assert(msdf.fontHandle);
    const auto fontHandle = msdf.fontHandle.get();

    msdfgen::Shape shape;

    double padding = 2.0;
    const double range = 4.0;
    const double unitsPerEm = static_cast<double>(msdf.metrics.emSize);
    const double scale = static_cast<double>(fontSize.height) / unitsPerEm;
    const msdfgen::Vector2 msdfScale(scale, scale);
    const msdfgen::Vector2 frame(fontSize.width, fontSize.height);

    std::vector<float> pixelBufferCache(fontSize.area() * 3);

    for (const GlyphId glyphId : glyphs) {
        shape.contours.clear();
        double advance;
        if (!msdfgen::loadGlyph(shape, fontHandle, static_cast<msdfgen::GlyphIndex>(glyphId),
                                &advance)) {
            RT_ERROR(L"Failed to load GlyphId[{}] using msdfgen", static_cast<size_t>(glyphId));
            return false;
        }
        if (shape.contours.empty()) {
            continue;
        }
        shape.normalize();
        msdfgen::edgeColoringSimple(shape, 3.0);

        const msdfgen::Shape::Bounds bounds = shape.getBounds();

        const double glyphWidth = bounds.r - bounds.l;
        const double glyphHeight = bounds.t - bounds.b;

        const msdfgen::Vector2 translate(padding - bounds.l * scale, padding - bounds.b * scale);
        const msdfgen::Projection projection(msdfgen::Vector2(scale, scale), translate);

        const double left = bounds.l * scale;
        const double right = bounds.r * scale;
        const double bottom = bounds.b * scale;
        const double top = bounds.t * scale;

        const Size bitmapSize = Size{
            .width = static_cast<Size::Type>(std::ceil(right - left) + 2 * padding),
            .height = static_cast<Size::Type>(std::ceil(top - bottom) + 2 * padding),
        };
        const size_t pixelsSize = bitmapSize.area() * 3;
        if (pixelsSize == 0) {
            RT_WARNING("Glyph[{}] has zero dimensions ({}x{})", static_cast<size_t>(glyphId), bitmapSize.width, bitmapSize.height);
            continue;
        }

        if (pixelBufferCache.size() <= pixelsSize) {
            pixelBufferCache.resize(pixelsSize);
        }
        msdfgen::BitmapRef<float, 3> msdf(pixelBufferCache.data(),
                                          static_cast<int>(bitmapSize.width),
                                          static_cast<int>(bitmapSize.height));
        msdfgen::generateMSDF(msdf, shape, projection, range);
        // saveToBmp(pixelBufferCache.data(), bitmapSize, "output_msdf.png");

        result.push_back(RasterizedGlyph{
            .id = glyphId,
            .bearingX = static_cast<float>(bounds.l * scale - range),
            .bearingY = static_cast<float>(bounds.t * scale - range),
            .advance = static_cast<float>(advance * scale),
            .atlasType = AtlasType::msdf,
            .bitmap =
                GlyphBitmap{
                    .size = bitmapSize,
                    .pixels = [size = bitmapSize,
                               data = pixelBufferCache.data()]() -> std::vector<std::byte> {
                        std::vector<std::byte> bytes(size.area() * 4);

                        const auto toByte = [](float v) -> std::byte {
                            v = std::clamp(v, 0.0f, 1.0f);
                            return static_cast<std::byte>(
                                static_cast<uint8_t>(std::round(v * 255.0f)));
                        };

                        for (size_t i = 0; i < size.area(); ++i) {
                            bytes[i * 4 + 0] = toByte(data[i * 3 + 0]);
                            bytes[i * 4 + 1] = toByte(data[i * 3 + 1]);
                            bytes[i * 4 + 2] = toByte(data[i * 3 + 2]);
                            bytes[i * 4 + 3] = std::byte{255};
                        }
                        return bytes;
                    }(),
                },
        });
    }

    return !result.empty();
}

} // namespace rendell_text