#pragma once
#include "openglbase.h"
#include "texture.h"

struct Material
{
public:
	std::string name;
	int ambientTextureId;
	int diffuseTextureId;
	int specularTextureId;
	float shininess;

	glm::vec3 ambient;
	glm::vec3 diffuse;

	Material(const std::string& name, int ambientTextureId, int diffuseTextureId, int specularTextureId, glm::vec3 ambient, glm::vec3 diffuse, float shininess);
};