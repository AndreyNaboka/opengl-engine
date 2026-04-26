#pragma once
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

class Shader {
public:
  unsigned int GetID() const { return _ID; }
  static std::optional<Shader> Create(const std::string &v,
                                      const std::string &f) {
    if (v.empty() || f.empty()) {
      log("Shaders code can't be empty");
      return std::nullopt;
    }
    return Shader(v, f);
  }

private:
  Shader(const std::string &vertCode, const std::string &fragCode) {
    log("Shader success created");
  };

private:
  unsigned int _ID = 0;
};
