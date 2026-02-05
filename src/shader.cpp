#include "logger.h"
#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

static std::string read_file(const std::string &path)
{
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

static unsigned int compile_shader(unsigned int type, const std::string &source)
{
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		char log[512];
		glGetShaderInfoLog(id, 512, nullptr, log);
		logger::error("Shader compilation failed:\n" + std::string(log));
	}
	return id;
}

shader::shader(const std::string &vertex_path, const std::string &fragment_path)
	: _id(glCreateProgram())
{
	std::string vertex_code = read_file(vertex_path);
	std::string fragment_code = read_file(fragment_path);

	unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_code);
	unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_code);

	glAttachShader(_id, vs);
	glAttachShader(_id, fs);
	glLinkProgram(_id);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void shader::use() const
{
	glUseProgram(_id);
}

int shader::get_uniform_location(const std::string &name) const
{
	auto it = _uniform_cache.find(name);
	if (it != _uniform_cache.end())
		return it->second;

	int loc = glGetUniformLocation(_id, name.c_str());
	_uniform_cache[name] = loc;
	return loc;
}

void shader::set_mat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
}

void shader::set_vec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(get_uniform_location(name), 1, &value[0]);
}

void shader::set_float(const std::string &name, float value) const
{
	glUniform1f(get_uniform_location(name), value);
}

void shader::set_int(const std::string &name, int value) const
{
	glUniform1i(get_uniform_location(name), value);
}
