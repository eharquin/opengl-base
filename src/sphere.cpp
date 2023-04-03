#include "sphere.h"

Sphere::Sphere()
{
	const float radius = 3.0f;
	const int sectorCount = 50;
	const int stackCount = 50;

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

			if ((i+j)% 2 == 0)
			{
				vertex.color = glm::vec3(0.0f, 0.0f, 1.0f);
			}
			else
			{
				vertex.color = glm::vec3(0.0f, 1.0f, 1.0f);
			}

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

IcoSphere::IcoSphere()
{
	constexpr float H_ANGLE = glm::pi<float>() / 180.0f * 72.0f;    // 72 degree = 360 / 5
	const float V_ANGLE = glm::atan(1.0f / 2);                      // elevation = 26.565 degree

	// compute "base" icosphere consitued of 12 vertices and 20 triangles

	std::vector<glm::vec3> baseVertices(12 * 3);
	int i1, i2;                                                     // indices
	float z, xy;                                                    // coord
	float hAngle1 = -glm::pi<float>() / 2.0f - H_ANGLE / 2.0f;      // start from -126 degree at first row
	float hAngle2 = -glm::pi<float>() / 2.0f;                       // start from -90 degree at second row

	// 1st top vertex
	glm::vec3 topVertexPosition = { 0.0f, 0.0f, radius };
	baseVertices.push_back(topVertexPosition);

	// compute 10 vertices at 1st and 2nd rows
	for (int i = 0; i <= 5; i++)
	{
		i1 = i * 3;                                                 // index for 1st row
		i2 = (i1 + 5) * 3;                                          // index for 2nd row

		z = radius * glm::sin(V_ANGLE);                             // elevation z
		xy = radius * glm::cos(V_ANGLE);                            // lenght on XY plane

		// 1st row
		glm::vec3 vertexPosition1 = { xy * glm::cos(hAngle1), xy * glm::sin(hAngle1), z };
		baseVertices.push_back(vertexPosition1);

		// 2nd row
		glm::vec3 vertexPosition2 = { xy * glm::cos(hAngle2), xy * glm::sin(hAngle2), -z };
		baseVertices.push_back(vertexPosition2);

		// compute next horizontal angles
		hAngle1 += H_ANGLE;
		hAngle2 += H_ANGLE;
	}

	// last bottom vertex
	i1 = 11 * 3;
	glm::vec3 bottomVertexPosition = { 0.0f, 0.0f, -radius };
	baseVertices.push_back(bottomVertexPosition);



	// compute subdivision
	const int subdivision = 3;
	
	std::vector<glm::vec3> tmpVertices;
	std::vector<unsigned int> tmpIndices;
	glm::vec3 v1, v2, v3;
	glm::vec3 newV1, newV2, newV3;
	unsigned int index;

	// iterate all subdivision levels
	for (int i = 0; i <= subdivision; i++)
	{
		// copy prev vertex/index arrays and clear
		tmpVertices = baseVertices;
		tmpIndices = indices_;
		vertices_.clear();
		indices_.clear();
		index = 0;

		// perform subdivision fo each triangle
		for (int j = 0; j < tmpIndices.size(); j += 3)
		{
			// get 3 vertices of a triangle
			v1 = tmpVertices[tmpIndices[j]];
			v2 = tmpVertices[tmpIndices[j + 1]];
			v3 = tmpVertices[tmpIndices[j + 2]];


			// compute 3 new vertices by spliting half on each edge
			//         v1       
			//        / \       
			// newV1 *---* newV3
			//      / \ / \     
			//    v2---*---v3   
			//       newV2      
			newV1 = computeHalfVertex(v1, v2);
			newV2 = computeHalfVertex(v2, v3);
			newV3 = computeHalfVertex(v1, v3);

			// add 4 new triangles to vertex array
			addVertices(v1, newV1, newV3);
			addVertices(newV1, v2, newV2);
			addVertices(newV1, newV2, newV3);
			addVertices(newV3, newV2, v3);

			// add indices of 4 new triangles
			addIndices(index, index + 1, index + 2);
			addIndices(index + 3, index + 4, index + 5);
			addIndices(index + 6, index + 7, index + 8);
			addIndices(index + 9, index + 10, index + 11);
			index += 12;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// find middle point of 2 vertices
// NOTE: new vertex must be resized, so the length is equal to the radius
///////////////////////////////////////////////////////////////////////////////
glm::vec3 IcoSphere::computeHalfVertex(const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 newV = glm::vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	float scale = radius / newV.length();
	newV *= scale;

	return newV;
}

void IcoSphere::addVertices(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	vertices_.push_back(Vertex{ v1, glm::vec3(1.0f) });
	vertices_.push_back(Vertex{ v2, glm::vec3(1.0f) });
	vertices_.push_back(Vertex{ v3, glm::vec3(1.0f) });
}

void IcoSphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
	indices_.push_back(i1);
	indices_.push_back(i2);
	indices_.push_back(i3);
}
