#include "sphere.h"

Sphere::Sphere()
{
	const float radius = 3.0f;
	const int sectorCount = 10;
	const int stackCount = 10;

	float x, y, z, xy;
	float nx, ny, nz, lenghtInv = 1.0f / radius;
	float s, t;

	constexpr float sectorStep = 2 * glm::pi<float>() / sectorCount;
	constexpr float stackStep = glm::pi<float>() / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * glm::cos(stackAngle);                       // r * cos(u)
		z = radius * glm::sin(stackAngle);                        // r * sin(u)

		// add (sectorCount + 1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			Vertex vertex;

			sectorAngle = j * sectorStep;                         // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * glm::cos(sectorAngle);                       // r * cos(u) * cos(v)
			y = xy * glm::sin(sectorAngle);                       // r * cos(u) * sin(v)
			
			vertex.position = glm::vec3(x, y, z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lenghtInv;
			ny = y * lenghtInv;
			nz = z * lenghtInv;

			vertex.normal = glm::vec3(nx, ny, nz);

			// vertex texture coordinates (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			
			vertex.uv = glm::vec2(t, s);

			vertices_.push_back(vertex);
		}

	}

	// generate CCW index list of sphere triangles
	// k1--k1+1
	// |  / |
	// | /  |
	// k2--k2+1

	int k1, k2;
	for (int i = 0; i < sectorCount; ++i)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// 
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices_.push_back(k1);
				indices_.push_back(k2);
				indices_.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount -1))
			{
				indices_.push_back(k1 + 1);
				indices_.push_back(k2);
				indices_.push_back(k2 + 1);
			}
		}
	}
}
