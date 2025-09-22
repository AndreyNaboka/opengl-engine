#include "shader.h"
#include "logger.h"

std::shared_ptr<shader> shader::create(const std::string& name, const std::string &vertex_code, const std::string &fragment_code) 
{
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

GLint shader::get_uniform_loc(const std::string &name)
{
	if (!_is_inited) {
		logger::error("Can't get uniform " + name + " from shader " + _name + ", cause not inited");
		return -1;
	}
	auto key = _uniforms.find(name);
	if (key != _uniforms.end()) {
		return key->second;
	}

	const GLint shader_uniform_location = glGetUniformLocation(_program, name.c_str());
	if (shader_uniform_location == -1) {
		logger::error("Can't find uniform " + name + " in shader " + _name + ", check uniform name");
		return -1;
	}

	_uniforms[name] = shader_uniform_location;
	return shader_uniform_location;
}

shader::shader(const std::string& name, const std::string &vertex_code, const std::string &fragment_code)
: _name(name)
, _fragment_code(fragment_code)
, _vertex_code(vertex_code) 
{
	GLint is_success = true;
	GLchar info_log[512];
	bool shader_failed = false;

	// vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* v_shader_ptr = vertex_code.c_str();
	glShaderSource(vertex_shader, 1, &v_shader_ptr, nullptr);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_success);
	if(!is_success) {
		shader_failed = true;
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		logger::error("Vertex shader " + name + " compile failed: " + std::string(info_log));
	};

	// fragment shader
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* f_shader_ptr = fragment_code.c_str();
	glShaderSource(fragment_shader, 1, &f_shader_ptr, nullptr);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_success);
	if(!is_success) {
		shader_failed = true;
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		logger::error("Fragment shader " + name + " compile failed: " + std::string(info_log));
	};

	// create program
	_program = glCreateProgram();
	glAttachShader(_program, vertex_shader);
	glAttachShader(_program, fragment_shader);
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &is_success);
	if(!is_success) {
		shader_failed = true;
		glGetProgramInfoLog(_program, 512, NULL, info_log);
		logger::error("Shader " + name + " link failed: " + std::string(info_log));
	}
	
	if (!shader_failed) 
		logger::info("Shader " + name + " create success");
	else
		logger::error("Shader " + name + " create failed");

	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);

	_is_inited = true;
}