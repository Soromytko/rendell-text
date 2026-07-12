#pragma once
#include <algorithm>
#include <cassert>
#include <optional>
#include <vector>

namespace rendell_text {
class Skyline final {
public:
    using data_t = size_t;

    struct Node {
        data_t x;
        data_t y;
    };

    struct InsertResult {
        // Left bottom corner.
        data_t x, y;
    };

    Skyline(data_t width, data_t height)
        : _width(width)
        , _height(height) {
        _nodes.push_back({0, 0});
    }

    std::optional<InsertResult> insert(data_t rectWidth, data_t rectHeight) {
        const auto maybeInsertion = findBestInsertion(rectWidth, rectHeight);
        if (!maybeInsertion) {
            return std::nullopt;
        }
        const auto insertion = maybeInsertion.value();
        assert(insertion.nodeIndex < _nodes.size());

        const InsertResult result{
            .x = _nodes[insertion.nodeIndex].x,
            .y = _nodes[insertion.nodeIndex].y + insertion.yOffset,
        };
        addSegment(insertion.nodeIndex, rectWidth, rectHeight, insertion.yOffset);
        return result;
    }

private:
    struct Insertion {
        data_t nodeIndex;
        data_t yOffset;
    };

    constexpr data_t getNodeWidth(size_t nodeIndex) const {
        const data_t x0 = _nodes[nodeIndex].x;
        const data_t x1 = nodeIndex + 1 < _nodes.size() ? _nodes[nodeIndex + 1].x : _width;
        return x1 - x0;
    }

    std::optional<Insertion> findBestInsertion(data_t rectWidth, data_t rectHeight) const {
        // At least one Node.
        assert(_nodes.size() > 0);

        int bestNodeIndex = -1;
        data_t yOffset = 0;

        for (size_t i = 0; i < _nodes.size(); i++) {
            const Node node = _nodes[i];
            data_t maxY = node.y + rectHeight;
            data_t width = getNodeWidth(i);
            for (size_t j = i + 1; j < _nodes.size() && width < rectWidth; j++) {
                const Node rightNode = _nodes[j];
                width += getNodeWidth(j);
                maxY = std::max(maxY, rightNode.y + rectHeight);
            }

            if (width < rectWidth || maxY > _height) {
                continue;
            }

            assert(bestNodeIndex < static_cast<int>(_nodes.size()));
            if (bestNodeIndex < 0 || node.y < _nodes[static_cast<uint32_t>(bestNodeIndex)].y) {
                yOffset = maxY - node.y - rectHeight;
                bestNodeIndex = static_cast<int>(i);
            }
        }

        if (bestNodeIndex >= 0) {
            return Insertion{
                .nodeIndex = static_cast<data_t>(bestNodeIndex),
                .yOffset = yOffset,
            };
        }

        return std::nullopt;
    }

    void addSegment(data_t nodeIndex, data_t rectWidth, data_t rectHeight, data_t yOffset) {
        assert(static_cast<size_t>(nodeIndex) < _nodes.size());
        assert(_width > 0);
        assert(_height > 0);

        const size_t index = static_cast<size_t>(nodeIndex);

        const Node newNode{
            .x = _nodes[index].x + rectWidth,
            .y = _nodes[index].y + yOffset,
        };
        _nodes[index].y += rectHeight + yOffset;
        _nodes.insert(_nodes.begin() + index + 1, newNode);

        for (auto it = _nodes.begin() + index + 2; it < _nodes.end(); it++) {
            if (it->x < newNode.x) {
                _nodes[index].y = it->y;
                it = _nodes.erase(it);
            } else {
                break;
            }
        }

        if (_nodes.size() > 0) {
            for (size_t i = 0; i < _nodes.size() - 1; i++) {
                for (size_t j = i + 1; j < _nodes.size(); j++) {
                    if (_nodes[i].x == _nodes[j].x || _nodes[i].y == _nodes[j].y) {
                        _nodes.erase(_nodes.begin() + j);
                        j--;
                    } else {
                        break;
                    }
                }
            }
        }
    }

    data_t _width, _height;
    std::vector<Node> _nodes{};
};
} // namespace rendell_text