#pragma once
#include <rendell_text/FontInstance.h>
#include <rendell_text/types.h>

namespace rendell_text {
struct TextRun {
    enum class Direction {
        LTR,
        RTL,
    };
    StringView text;
    FontInstance fontInstance;
    Direction direction = Direction::LTR;
};
} // namespace rendell_text