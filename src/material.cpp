#include "material.h"

Material::Material(const std::string& name, int ambientTextureId, int diffuseTextureId, int specularTextureId, glm::vec3 ambient, glm::vec3 diffuse, float shininess)
	: name(name), ambientTextureId(ambientTextureId), diffuseTextureId(diffuseTextureId), specularTextureId(specularTextureId), ambient(ambient), diffuse(diffuse), shininess(shininess)
{
}