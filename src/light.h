#include "openglbase.h"

struct PointLight
{
	PointLight(const glm::vec3& color, const glm::vec3& position, const glm::vec3& specular, float constant, float linear, float quadratic)
		: color(color), position(position), specular(specular), constant(constant), linear(linear), quadratic(quadratic) {}

	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;

};

struct DirectionalLight
{
	DirectionalLight(const glm::vec3& color, const glm::vec3& direction, const glm::vec3& specular)
		: color(color), direction(direction), specular(specular) {}

	glm::vec3 color;
	glm::vec3 direction;
	glm::vec3 specular;

};

struct SpotLight
{
	SpotLight(const glm::vec3& color, const glm::vec3& direction, const glm::vec3& position, const glm::vec3& specular, float inner, float outer, float constant, float linear, float quadratic)
	: color(color), direction(direction), position(position), specular(specular), inner(inner), outer(outer), constant(constant), linear(linear), quadratic(quadratic) {}

	glm::vec3 color;
	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 specular;

	float inner;
	float outer;

	float constant;
	float linear;
	float quadratic;

};