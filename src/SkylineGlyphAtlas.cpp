#include <SkylineGlyphAtlas.h>

#include <algorithm>
#include <cassert>

namespace rendell_text {
SkylineGlyphAtlas::SkylineGlyphAtlas(Size size)
    : _size(size) {
    assert(size.width > 0 && size.height > 0);
    _skyline.push_back({0, 0, size.width});
    _pixels.resize(_size.width * _size.height * 3);
}

IGlyphAtlas::Info SkylineGlyphAtlas::getGlyphInfo(GlyphKey key) const {
    const auto it = _glyphs.find(key);
    assert(it != _glyphs.end());
    return makeInfo(it->second.uv, it->second.glyph);
}

bool SkylineGlyphAtlas::findGlyphInfo(GlyphKey key, Info &info) const {
    const auto it = _glyphs.find(key);
    if (it == _glyphs.end()) {
        return false;
    }
    info = makeInfo(it->second.uv, it->second.glyph);
    return true;
}

PixelsRef SkylineGlyphAtlas::getGlyphPixels(GlyphKey key) const {
    const auto it = _glyphs.find(key);
    assert(it != _glyphs.end());
    return it->second.glyph.bitmap.pixels;
}

bool SkylineGlyphAtlas::resize(Size size) {
    assert(false);
    return false;
}

bool SkylineGlyphAtlas::insert(const RasterizedGlyph &glyph, FontInstance fontInstance) {
    if (glyph.bitmap.size.width > _size.width || glyph.bitmap.size.height > _size.height) {
        return false;
    }

    Size::Type yOffset;
    const auto bestNodeIndex = findBestNodeIndex(glyph.bitmap.size, yOffset);
    if (bestNodeIndex < 0) {
        return false;
    }
    assert(bestNodeIndex < _skyline.size());

    auto maybeUV = insertGlyph(glyph.bitmap, static_cast<Size::Type>(bestNodeIndex));
    if (!maybeUV) {
        return false;
    }
    addSkylineSegment(bestNodeIndex, glyph.bitmap.size, yOffset);
    _glyphs.insert({GlyphKey{
                        .id = glyph.id,
                        .fontInstance = fontInstance,
                    },
                    GlyphData{
                        .uv = maybeUV.value(),
                        .glyph = glyph,
                    }});
    return true;
}

SkylineGlyphAtlas::Info SkylineGlyphAtlas::makeInfo(UV uv, const RasterizedGlyph &glyph) const {
    return Info{
        .u0 = uv.u0,
        .u1 = uv.u1,
        .v0 = uv.v0,
        .v1 = uv.v1,
        .bearingX = glyph.bearingX,
        .bearingY = glyph.bearingY,
        .advance = glyph.advance,
        .size = glyph.bitmap.size,
    };
}

int SkylineGlyphAtlas::findBestNodeIndex(Size size, Size::Type &yOffset) const {
    // At least one Node.
    assert(_skyline.size() > 0);

    int bestNodeIndex = -1;

    for (size_t i = 0; i < _skyline.size(); i++) {
        const SkylineNode node = _skyline[i];
        Size::Type maxY = node.y + size.height;
        Size::Type width = node.width;
        for (size_t j = i; j < _skyline.size() && width < size.width; j++) {
            const SkylineNode rightNode = _skyline[j];
            width += rightNode.width;
            maxY = std::max(maxY, static_cast<Size::Type>(rightNode.y + size.height));
        }

        if (width < size.width || maxY > _size.height) {
            continue;
        }

        assert(bestNodeIndex < _skyline.size());
        if (bestNodeIndex < 0 || node.y < _skyline[static_cast<uint32_t>(bestNodeIndex)].y) {
            yOffset = maxY - node.y;
            bestNodeIndex = static_cast<int>(i);
        }
    }

    return bestNodeIndex;
}

std::optional<SkylineGlyphAtlas::UV> SkylineGlyphAtlas::insertGlyph(const GlyphBitmap &bitmap,
                                                                    size_t nodeInsertIndex) {
    assert(nodeInsertIndex < _skyline.size());

    const SkylineNode bestNode = _skyline[nodeInsertIndex];
    assert(bestNode.x + bitmap.size.width < _size.width);
    assert(bestNode.y + bitmap.size.height < _size.height);

    for (size_t y = 0; y < bitmap.size.height; y++) {
        const size_t atlasIndex = (bestNode.y + y) * _size.width + bestNode.x;
        const size_t glyphIndex = y * bitmap.size.width;
        memcpy(&_pixels[atlasIndex], &bitmap.pixels[glyphIndex], bitmap.size.width);
    }

    return UV{
        .u0 = static_cast<float>(bestNode.x) / _size.width,
        .v0 = static_cast<float>(bestNode.y) / _size.height,
        .u1 = static_cast<float>(bestNode.x + bitmap.size.width) / _size.width,
        .v1 = static_cast<float>(bestNode.y + bitmap.size.height) / _size.height,
    };
}

void SkylineGlyphAtlas::addSkylineSegment(Size::Type index, Size size, Size::Type yOffset) {
    assert(static_cast<size_t>(index) < _skyline.size());
    assert(size.width > 0);
    assert(size.height > 0);

    _skyline[index].y += size.height + yOffset;
    const SkylineNode newNode{
        .x = static_cast<Size::Type>(_skyline[index].x + size.width + yOffset),
        .y = _skyline[static_cast<size_t>(index)].y,
        .width = {},
    };
    _skyline.insert(_skyline.begin() + index + 1, newNode);

    for (auto it = _skyline.begin() + index + 2; it < _skyline.end(); it++) {
        if (it->x < newNode.x) {
            _skyline[index].y = it->y;
            it = _skyline.erase(it);
        } else {
            break;
        }
    }

    if (_skyline.size() > 0) {
        for (size_t i = 0; i < _skyline.size() - 1; i++) {
            for (size_t j = i + 1; j < _skyline.size(); j++) {
                if (_skyline[i].x == _skyline[j].x || _skyline[i].y == _skyline[j].y) {
                    _skyline.erase(_skyline.begin() + j);
                    j--;
                } else {
                    break;
                }
            }
        }
    }
}
} // namespace rendell_text