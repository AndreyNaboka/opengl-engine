#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

class Logger
{
public:
	enum Level
	{
		INFO,
		WARNING,
		ERROR
	};
	static void Info(const std::string &message) { Log(INFO, "INFO", message); }
	static void Warn(const std::string &message) { Log(WARNING, "WARN", message); }
	static void Error(const std::string &message) { Log(ERROR, "ERROR", message); }

private:
	static void Log(Level level, const std::string &label, const std::string &msg)
	{
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

		std::tm tm = *std::localtime(&time_t);
		std::cout
			<< std::put_time(&tm, "%H:%M:%S")
			<< '.' << std::setfill('0') << std::setw(3) << ms.count()
			<< " [" << label << "] "
			<< msg << std::endl;
	}
};
