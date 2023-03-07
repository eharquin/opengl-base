#pragma once
#include "openglbase.h"

#include <fstream>

struct Shader
{
private:
	GLuint id_;

public:
	Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath);
	~Shader();

	// delete copy constructor
	Shader(const Shader&) = delete;
	// delete move constructor
	Shader(Shader&& other) = delete;
	// delete assignment operators
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&& other) = delete;

	GLuint id() const { return id_; }

	std::string readShaderFile(const std::string& filePath);

	void use();

	void uniform1i(const std::string& name, int value);
	void uniform1f(const std::string& name, float value);
	void uniformMat4(const std::string& name, glm::mat4 value);
	void uniformVec3(const std::string& name, glm::vec3 value);
	void uniformVec4(const std::string& name, glm::vec4 value);
};