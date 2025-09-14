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

shader::shader(const std::string& name, const std::string &vertex_code, const std::string &fragment_code)
: _name(name)
, _fragment_code(fragment_code)
, _vertex_code(vertex_code) 
{
	GLint is_success = true;
	GLchar info_log[512];
	int errors = 0;

	// vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* v_shader_ptr = vertex_code.c_str();
	glShaderSource(vertex_shader, 1, &v_shader_ptr, nullptr);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_success);
	if(!is_success)
	{
		errors++;
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		logger::error("Vertex shader " + name + " compile failed: " + std::string(info_log));
	};

	// fragment shader
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* f_shader_ptr = fragment_code.c_str();
	glShaderSource(fragment_shader, 1, &f_shader_ptr, nullptr);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_success);
	if(!is_success)
	{
		errors++;
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		logger::error("Fragment shader " + name + " compile failed: " + std::string(info_log));
	};

	// create program
	_program = glCreateProgram();
	glAttachShader(_program, vertex_shader);
	glAttachShader(_program, fragment_shader);
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &is_success);
	if(!is_success)
	{
		errors++;
		glGetProgramInfoLog(_program, 512, NULL, info_log);
		logger::error("Shader " + name + " link failed: " + std::string(info_log));
	}
	
	if (!errors) logger::info("Shader " + name + " create success");

	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);
}