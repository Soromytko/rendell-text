#pragma once
#include <iostream>
#include <format>
#include <logx/logx.h>

namespace rendell_text
{
	class RTLogger final : public logx::Logger
	{
	public:
		RTLogger();
		~RTLogger() = default;

	private:
		const char* getLevelName_Unsafe(logx::Level level) const override;
	};

	logx::Logger* get_logger();
}

#define RT_CRITICAL(formatStr, ...) rendell_text::get_logger()->critical(std::format(formatStr, __VA_ARGS__))
#define RT_ERROR(formatStr, ...) rendell_text::get_logger()->error(std::format(formatStr, __VA_ARGS__))
#define RT_WARNING(format, ...) rendell_text::get_logger()->warning(std::format(formatStr, __VA_ARGS__))
#define RT_INFO(formatStr, ...) rendell_text::get_logger()->info(std::format(formatStr, __VA_ARGS__))
#define RT_DEBUG(formatStr, ...) rendell_text::get_logger()->debug(std::format(formatStr, __VA_ARGS__))
#define RT_TRACE(formatStr, ...) rendell_text::get_logger()->trace(std::format(formatStr, __VA_ARGS__))
