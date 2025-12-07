#include <SkylineGlyphAtlas.h>

#include <algorithm>

namespace rendell_text {
SkylineGlyphAtlas::SkylineGlyphAtlas(uint32_t width, uint32_t height)
    : _width(width)
    , _height(height) {
    assert(_width > 0);
    assert(_height > 0);
    _skyline.push_back({0, 0, _width});
    _pixels.resize(_width * _height * 3);
}

size_t SkylineGlyphAtlas::getVersion() const {
    return _version;
}

uint32_t SkylineGlyphAtlas::getWidth() const {
    return _width;
}

uint32_t SkylineGlyphAtlas::getHeight() const {
    return _height;
}

const std::vector<rendell::byte_t> &SkylineGlyphAtlas::getPixels() const {
    return _pixels;
}

bool SkylineGlyphAtlas::tryInsert(const GlyphBitmap &glyph, UV &uv) {
    if (glyph.width > _width || glyph.height > _height) {
        return false;
    }

    uint32_t yOffset;
    const auto bestNodeIndex = findBestNodeIndex(glyph, yOffset);
    if (bestNodeIndex < 0) {
        return false;
    }
    assert(bestNodeIndex < _skyline.size());

    insertGlyph(glyph, static_cast<uint32_t>(bestNodeIndex), uv);
    addSkylineSegment(bestNodeIndex, glyph.width, glyph.height, yOffset);
    _version++;
    return true;
}

int SkylineGlyphAtlas::findBestNodeIndex(const GlyphBitmap &glyph, uint32_t &yOffset) const {
    // At least one Node.
    assert(_skyline.size() > 0);

    int bestNodeIndex = -1;

    for (size_t i = 0; i < _skyline.size(); i++) {
        const SkylineNode node = _skyline[i];
        uint32_t maxY = node.y + glyph.height;
        uint32_t width = node.width;
        for (size_t j = i; j < _skyline.size() && width < glyph.width; j++) {
            const SkylineNode rightNode = _skyline[j];
            width += rightNode.width;
            maxY = std::max(maxY, rightNode.y + glyph.height);
        }

        if (width < glyph.width || maxY > _height) {
            continue;
        }

        assert(bestNodeIndex < _skyline.size());
        if (bestNodeIndex < 0 || node.y < _skyline[static_cast<uint32_t>(bestNodeIndex)].y) {
            yOffset = maxY - node.y;
            bestNodeIndex = i;
        }
    }

    return bestNodeIndex;
}

void SkylineGlyphAtlas::insertGlyph(const GlyphBitmap &glyph, size_t nodeInsertIndex, UV &uv) {
    assert(nodeInsertIndex < _skyline.size());

    const SkylineNode bestNode = _skyline[nodeInsertIndex];
    assert(bestNode.x + glyph.width < _width);
    assert(bestNode.y + glyph.height < _height);

    for (size_t y = 0; y < glyph.height; y++) {
        const size_t atlasIndex = (bestNode.y + y) * _width + bestNode.x;
        const size_t glyphIndex = y * glyph.width;
        memcpy(&_pixels[atlasIndex], &glyph.pixels[glyphIndex], glyph.width);
    }

    uv.u0 = bestNode.x / _width;
    uv.v0 = bestNode.y / _height;
    uv.u1 = (bestNode.x + glyph.width) / _width;
    uv.v1 = (bestNode.y + glyph.height) / _height;
}

void SkylineGlyphAtlas::addSkylineSegment(size_t index, uint32_t width, uint32_t height,
                                          uint32_t yOffset) {
    assert(index < _skyline.size());
    assert(width > 0);
    assert(height > 0);

    _skyline[index].y += height + yOffset;
    const SkylineNode newNode{_skyline[index].x + width + yOffset, _skyline[index].y};
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