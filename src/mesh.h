#pragma once
#include "openglbase.h"
#include "shader.h"
#include "texture.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<Texture>&& textures);

	~Mesh();

	//Delete the copy constructor/assignment.
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	// define move constructor
	Mesh(Mesh&& other);
	// define move assignement operator
	Mesh& operator=(Mesh&& other);

	void Draw (const Shader& shader) const;

private:
	GLuint VAO, VBO, EBO;
	void setup();

	void release();
};