#pragma once
#include <iostream>
#include <sstream>
#include <string>

class Logger {
public:
  static Logger &GetInstance() {
    static Logger instance;
    return instance;
  }

  void Info(const std::string &msg) {
    std::cout << "[INFO] " << msg << std::endl;
  }

  void Debug(const std::string &msg) {
    std::cout << "[DEBUG] " << msg << std::endl;
  }

private:
  Logger() = default;
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
};

template <typename... Args> inline void LogInfo(Args &&...args) {
  std::ostringstream oss;
  (oss << ... << std::forward<Args>(args));
  Logger::GetInstance().Info(oss.str());
}

#ifdef ENGINE_DEBUG
template <typename... Args> inline void _LogDebugImpl(Args &&...args) {
  std::ostringstream oss;
  (oss << ... << std::forward<Args>(args));
  Logger::GetInstance().Debug(oss.str());
}

#define LOG_DEBUG(...) _LogDebugImpl(__VA_ARGS__)
#else
#define LOG_DEBUG(...)                                                         \
  do {                                                                         \
  } while (0)
#endif
