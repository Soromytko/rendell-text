#include <cassert>
#include <logging.h>

namespace rendell_text {
RTLogger::RTLogger()
    : logx::Logger() {
}

const char *RTLogger::getLevelName_Unsafe(logx::Level level) const {
    static const std::unordered_map<logx::Level, const char *> levelNames{
        {logx::Level::critical, "RENDELL_TEXT::CRITICAL"},
        {logx::Level::error, "RENDELL_TEXT::ERROR"},
        {logx::Level::warning, "RENDELL_TEXT::WARNING"},
        {logx::Level::info, "RENDELL_TEXT::INFO"},
        {logx::Level::debug, "RENDELL_TEXT::DEBUG"},
        {logx::Level::trace, "RENDELL_TEXT::TRACE"},
    };

    auto it = levelNames.find(level);
    if (it != levelNames.end()) {
        return it->second;
    }

    return nullptr;
}

static std::unique_ptr<logx::Logger> s_logger{nullptr};

logx::Logger *get_logger() {
    if (!s_logger) {
        s_logger = std::make_unique<logx::Logger>();
    }
    return s_logger.get();
}

} // namespace rendell_text