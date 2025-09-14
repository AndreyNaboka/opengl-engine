#include "shader.h"
#include "logger.h"

std::shared_ptr<shader> shader::create(const std::string& name, const std::string &vertex_code, const std::string &fragment_code) {
   if (vertex_code.empty()) {
      logger::error("Shader " + name + ": vertex code is empty");
      return nullptr;
   }
   if (fragment_code.empty()) {
      logger::error("Shader " + name + ": fragment code is empty");
      return nullptr;
   }
   logger::info("Create shader " + name);
   return std::shared_ptr<shader>(new shader(name, vertex_code, fragment_code));
}

shader::shader(const std::string& name, const std::string &vertex_code, const std::string &fragment_code)
: _name(name)
, _fragment_code(fragment_code)
, _vertex_code(vertex_code) {

}