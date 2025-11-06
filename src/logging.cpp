#include <cassert>
#include <logging.h>

namespace rendell_text {
RTLogger::RTLogger()
    : logr::Logger() {
}

const char *RTLogger::getLevelName_Unsafe(logr::Level level) const {
    static const std::unordered_map<logr::Level, const char *> levelNames{
        {logr::Level::critical, "RENDELL_TEXT::CRITICAL"},
        {logr::Level::error, "RENDELL_TEXT::ERROR"},
        {logr::Level::warning, "RENDELL_TEXT::WARNING"},
        {logr::Level::info, "RENDELL_TEXT::INFO"},
        {logr::Level::debug, "RENDELL_TEXT::DEBUG"},
        {logr::Level::trace, "RENDELL_TEXT::TRACE"},
    };

    auto it = levelNames.find(level);
    if (it != levelNames.end()) {
        return it->second;
    }

    return nullptr;
}

static std::unique_ptr<logr::Logger> s_logger{nullptr};

logr::Logger *get_logger() {
    if (!s_logger) {
        s_logger = std::make_unique<logr::Logger>();
    }
    return s_logger.get();
}

} // namespace rendell_text