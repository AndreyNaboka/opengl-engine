#include "Logger.h"
#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

static std::string ReadFile(const std::string &path)
{
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

static unsigned int CompileShader(unsigned int type, const std::string &source)
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
		Logger::Error("Shader compilation failed:\n" + std::string(log));
	}
	return id;
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
	: _id(glCreateProgram())
{
	std::string vertexCode = ReadFile(vertexPath);
	std::string fragmentCode = ReadFile(fragmentPath);

	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexCode);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

	glAttachShader(_id, vs);
	glAttachShader(_id, fs);
	glLinkProgram(_id);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Shader::Use() const
{
	glUseProgram(_id);
}

int Shader::GetUniformLocation(const std::string &name) const
{
	auto it = _uniformCache.find(name);
	if (it != _uniformCache.end())
		return it->second;

	int loc = glGetUniformLocation(_id, name.c_str());
	_uniformCache[name] = loc;
	return loc;
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetFloat(const std::string &name, float value) const
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(GetUniformLocation(name), value);
}
