#include <SkylineGlyphAtlas.h>

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

bool SkylineGlyphAtlas::tryInsert(const GlyphBitmap &glyph, GlyphInfo &glyphInfo) {
    if (glyph.width > _width || glyph.height > _height) {
        return false;
    }

    const auto bestNodeIndex = findBestNodeIndex(glyph);
    if (bestNodeIndex < 0) {
        return false;
    }
    assert(bestNodeIndex < _skyline.size());

    insertGlyph(glyph, static_cast<uint32_t>(bestNodeIndex));
    addSkylineSegment(bestNodeIndex, glyph.width, glyph.height);
    _version++;
    return true;
}

int SkylineGlyphAtlas::findBestNodeIndex(const GlyphBitmap &glyph) const {
    // At least one Node.
    assert(_skyline.size() > 0);

    int bestNodeIndex = -1;

    for (size_t i = 0; i < _skyline.size(); i++) {
        const SkylineNode node = _skyline[i];
        if (node.y + glyph.height > _height) {
            continue;
        }

        if (node.x + glyph.width < node.width) {
            bestNodeIndex = i;
            continue;
        }
        bool isEnough = false;
        for (size_t j = i + 1; j < _skyline.size(); j++) {
            const SkylineNode node2 = _skyline[j];
            if (node.x + glyph.width < node2.x) {
                isEnough = true;
                break;
            }
            if (node2.y > node.y) {
                if (node.x + glyph.width > node2.x) {
                    isEnough = false;
                    break;
                }
            }
        }
        if (!isEnough) {
            continue;
        }
        assert(bestNodeIndex < _skyline.size());
        if (bestNodeIndex < 0 || node.y < _skyline[bestNodeIndex].y) {
            bestNodeIndex = i;
        }
    }

    return bestNodeIndex;
}

void SkylineGlyphAtlas::insertGlyph(const GlyphBitmap &glyph, size_t nodeInsertIndex) {
    assert(nodeInsertIndex < _skyline.size());

    const SkylineNode bestNode = _skyline[nodeInsertIndex];
    assert(bestNode.x + glyph.width < _width);
    assert(bestNode.y + glyph.height < _height);

    for (size_t y = 0; y < glyph.height; y++) {
        const size_t atlasIndex = (bestNode.y + y) * _width + bestNode.x;
        const size_t glyphIndex = y * glyph.width;
        memcpy(&_pixels[atlasIndex], &glyph.pixels[glyphIndex], glyph.width);
    }

    const SkylineNode insertNode = _skyline[nodeInsertIndex];
    _skyline[nodeInsertIndex].y += glyph.height;
    if (nodeInsertIndex > 0 && _skyline[nodeInsertIndex].y == _skyline[nodeInsertIndex - 1].y) {
        _skyline.erase(_skyline.begin() + nodeInsertIndex);
        nodeInsertIndex--;
    }
    if (insertNode.x + glyph.width < insertNode.width) {
        auto it = _skyline.insert(_skyline.begin() + nodeInsertIndex, insertNode);
        it->x += glyph.width;
        it->width = insertNode.width - glyph.width;
    } else {
        const uint32_t newX = insertNode.x + glyph.width;
        if (nodeInsertIndex + 1 == _skyline.size()) {
            _skyline.insert(_skyline.begin() + nodeInsertIndex + 1,
                            {insertNode.x + glyph.width, insertNode.y, _width - insertNode.x});
        } else {
            for (auto it = _skyline.begin() + nodeInsertIndex + 1; it != _skyline.end(); it++) {
                if (it->x == newX) {
                    break;
                }
                if (it->x < newX) {
                    it = _skyline.erase(it);
                } else if (it->x > newX) {
                    _skyline.insert(_skyline.begin() + nodeInsertIndex + 1,
                                    {newX, insertNode.y, it->x - newX});
                }
            }
        }
    }

    SkylineNode newPoint = _skyline[nodeInsertIndex];
    newPoint.x += glyph.width;
    _skyline.insert(_skyline.begin() + nodeInsertIndex + 1, newPoint);

    _skyline[nodeInsertIndex].y += glyph.height;
    if (_skyline[nodeInsertIndex - 1].y == _skyline[nodeInsertIndex].y) {
        _skyline.erase(_skyline.begin() + nodeInsertIndex);
    }

    _skyline.insert(_skyline.begin() + nodeInsertIndex + 1, {});
}

void SkylineGlyphAtlas::addSkylineSegment(size_t index, uint32_t x, uint32_t y, uint32_t width,
                                          uint32_t height) {
}
} // namespace rendell_text