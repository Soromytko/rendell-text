#pragma once
#include <rendell_text/types.h>

namespace rendell_text {
template <class T> inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher{};
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // namespace rendell_text

namespace std {
template <> struct hash<rendell_text::GlyphKey> {
    size_t operator()(rendell_text::GlyphKey key) const {
        size_t seed = 0;
        rendell_text::hash_combine(seed, key.id);
        rendell_text::hash_combine(seed, key.fontInstance.size.width);
        rendell_text::hash_combine(seed, key.fontInstance.size.height);
        return seed;
    }
};
} // namespace std