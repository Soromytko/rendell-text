#pragma once
#include <rendell/rendell.h>

namespace rendell_text {
class RenderingServer final {
private:
    RenderingServer();

public:
    ~RenderingServer() = default;

    static void init();
    static void release();
    static RenderingServer *getInstance();
};
} // namespace rendell_text
