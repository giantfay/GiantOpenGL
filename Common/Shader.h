#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Shader
{
public:
	Shader(const GLchar* vertShaderPath, const GLchar* fragShaderPath);

	void Use();
	void SetBool(const std::string& name, bool value) const;
	void SetFloat(const std::string& name, GLfloat value) const;
	void SetInt(const std::string& name, GLint value) const;

	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetMat4(const std::string& name, const glm::mat4& value) const;
	GLuint shaderProgram;
};

Shader::Shader(const GLchar * vertShaderPath, const GLchar * fragShaderPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertShaderPath);
		fShaderFile.open(fragShaderPath);

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		
		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		std::cout << vertexCode << std::endl << std::endl << fragmentCode << std::endl;
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Error: failed to read shader file" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "Error: vertex shader compile failed!\n " << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
	glCompileShader(fragmentShader);
	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "Error: fragment shader compile failed!\n " << infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "Error: shaderProgram link failed!\n " << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

inline void Shader::Use()
{
	glUseProgram(shaderProgram);
}

inline void Shader::SetBool(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (GLint)value);
}

inline void Shader::SetFloat(const std::string & name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

inline void Shader::SetInt(const std::string & name, GLint value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

inline void Shader::SetVec2(const std::string & name, const glm::vec2 & value) const
{
	glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

inline void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

inline void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
