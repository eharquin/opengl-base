#pragma once
#include "geometry.h"

class Sphere : public Geometry
{
public:
	Sphere();
};

class IcoSphere : public Geometry
{
public:
	IcoSphere();

	const float radius = 3.0f;

private:
	glm::vec3 computeHalfVertex(const glm::vec3& v1, const glm::vec3& v2);
	void addVertices(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
};