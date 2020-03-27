#pragma once
#include <string>

namespace CU
{
	enum class PrintTextColor
	{
		Red,
		Green,
		Blue,
		Yellow,
		White
	};
	enum class LogType
	{
		Log,
		Warning,
		Error,
		Success
	};

	void Log(const std::string& aMessage, const LogType aLogType = LogType::Log);
}