#pragma once
#include <initializer_list>
#include <iostream>
#include <string>

class Logger {
public:
  static Logger &GetInstance() {
    static Logger l;
    return l;
  }
  void out(const std::string &t) { std::cout << t << std::endl; }
  void out(const std::string &t, std::initializer_list<double> numbers) {
    std::cout << t;
    for (auto num : numbers)
      std::cout << " " << num;
    std::cout << std::endl;
  }
};

inline void LogInfo(const std::string &t) { Logger::GetInstance().out(t); }

inline void LogInfo(const std::string &t, std::initializer_list<double> numbers) {
  Logger::GetInstance().out(t, numbers);
}

