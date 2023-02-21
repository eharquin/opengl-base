#include "shader.h"

Shader::Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath)
{
	GLuint fragmentShader, vertexShader;
	int success;
	char infoLog[512];

	std::string vertexShaderSourceString = readShaderFile(vertexFilePath);
	std::string fragmentShaderSourceString = readShaderFile(fragmentFilePath);

	const char* vertexShaderSource = vertexShaderSourceString.c_str();
	const char* fragmentShaderSource = fragmentShaderSourceString.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// sets the source code in shader with one null terminated string
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	id_ = glCreateProgram();
	glAttachShader(id_, vertexShader);
	glAttachShader(id_, fragmentShader);
	glLinkProgram(id_);

	glGetProgramiv(id_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id_, 512, NULL, infoLog);
		throw std::runtime_error(infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(id_);
}

std::string Shader::readShaderFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate);
	if (!file.is_open())
	{
		throw std::runtime_error("failed to open shader file: " + filePath);
	}

	std::string shaderCode;
	shaderCode.resize(file.tellg());

	file.seekg(0);
	file.read(shaderCode.data(), shaderCode.size());
	file.close();

	return shaderCode;
}

void Shader::use()
{
	glUseProgram(id_);
}

void Shader::uniform1i(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}
