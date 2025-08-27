// Logger.h
#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>

class Logger {
public:
		enum Level {
			INFO,
			WARNING,
			ERROR
		};

 // Основные методы
 static void info(const std::string& message) 
 {
	log(INFO, "INFO", message, "\033[32m"); // зелёный
 }

 static void warn(const std::string& message) 
 {
	log(WARNING, "WARN", message, "\033[33m"); // жёлтый
 }

 static void error(const std::string& message) 
 {
	log(ERROR, "ERROR", message, "\033[31m"); // красный
 }

private:
 static void log(Level level, const std::string& label, const std::string& msg, const std::string& color) 
 {
			// Получаем текущее время
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

		// Форматируем время: HH:MM:SS.mmm
		std::tm tm = *std::localtime(&time_t);
		std::cout << "\033[2m" // серый цвет для времени
			<< std::put_time(&tm, "%H:%M:%S")
			<< '.' << std::setfill('0') << std::setw(3) << ms.count()
			<< "\033[0m " // сброс цвета

			<< color << "[" << label << "]\033[0m "
			<< msg << std::endl;
 }
};
