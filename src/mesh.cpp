#include "mesh.h"

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, unsigned int materialId)
	: vertices(std::move(vertices)), indices(std::move(indices)), materialId(materialId)
{
	setup();
}

Mesh::~Mesh()
{
	release();
}

Mesh::Mesh(Mesh&& other)
	: VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), vertices(other.vertices), indices(other.indices), materialId(other.materialId)
{
	other.VAO = GL_NONE;
	other.VBO = GL_NONE;
	other.EBO = GL_NONE;
}

Mesh& Mesh::operator=(Mesh&& other)
{
	//ALWAYS check for self-assignment.
	if (this != &other)
	{
		release();
		//obj_ is now 0.
		std::swap(VAO, other.VAO);
		std::swap(VBO, other.VBO);
		std::swap(EBO, other.EBO);

		// manage vectors ...
	}

	return *this;
}

void Mesh::Draw(const Shader& shader, const Material& material) const
{

	shader.uniform1i("material.has_ambient_texture", material.ambientTextureId);
	if(material.ambientTextureId != -1)
		shader.uniform1i("material.texture_ambient",  material.ambientTextureId);
	
	shader.uniform1i("material.has_diffuse_texture", material.diffuseTextureId);
	if (material.diffuseTextureId != -1)
		shader.uniform1i("material.texture_diffuse",  material.diffuseTextureId);

	shader.uniform1i("material.has_specular_texture", material.specularTextureId);
	if (material.specularTextureId != -1)
		shader.uniform1i("material.texture_specular", material.specularTextureId);

	shader.uniform1i("material.shininess", 32);


	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setup()
{
	glCreateVertexArrays(1, &VAO);


	glCreateBuffers(1, &VBO);
	glNamedBufferData(VBO, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glCreateBuffers(1, &EBO);
	glNamedBufferData(EBO, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));

	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribBinding(VAO, 1, 0);

	glVertexArrayElementBuffer(VAO, EBO);
}

void Mesh::release()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &EBO);
}

