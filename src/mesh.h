#pragma once
#include "openglbase.h"
#include "shader.h"
#include "material.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;

	inline bool operator==(const Vertex& other) const {
		return position == other.position && uv == other.uv && normal == other.normal;
	}
};

// implementation of hash calculation for Vertex
template<> struct std::hash<Vertex>
{
	size_t operator()(Vertex const& vertex) const
	{
		return ((std::hash<glm::vec3>()(vertex.position) ^
			(std::hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
			(std::hash<glm::vec2>()(vertex.uv) << 1);
	}
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int materialId;

	Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, unsigned int materialId);

	~Mesh();

	//Delete the copy constructor/assignment.
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	// define move constructor
	Mesh(Mesh&& other);
	// define move assignement operator
	Mesh& operator=(Mesh&& other);

	void Draw (const Shader& shader, const Material& material) const;

private:
	GLuint VAO, VBO, EBO;
	void setup();

	void release();
};