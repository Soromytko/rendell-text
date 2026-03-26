#pragma once
#include <rendell_text/FontInstance.h>
#include <rendell_text/types.h>

namespace rendell_text {
struct TextRun {
    enum class Direction {
        LTR,
        RTL,
        TTB,
    };
    StringView text;
    FontInstance fontInstance;
    Direction direction = Direction::LTR;
    Script script;
    Language language;
};
} // namespace rendell_text