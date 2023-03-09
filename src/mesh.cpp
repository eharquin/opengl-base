#include "mesh.h"

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<Texture>&& textures)
	: vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
{
	setup();
}

Mesh::~Mesh()
{
	release();
}

Mesh::Mesh(Mesh&& other)
	: VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), vertices(other.vertices), indices(other.indices), textures(std::move(other.textures))
{
	other.VAO = GL_NONE;
	other.VBO = GL_NONE;
	other.EBO = GL_NONE;

	other.vertices = std::vector<Vertex>();
	other.indices = std::vector<unsigned int>();
	other.textures = std::vector<Texture>();
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

void Mesh::Draw(const Shader& shader) const
{
	unsigned int diffuseTextureCount = 0;
	unsigned int specularTextureCount = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		textures[i].bind(GL_TEXTURE0 + i);

		if(textures[i].type() == "texture_diffuse")
			shader.uniform1i("material.texture_diffuse[" + std::to_string(diffuseTextureCount++) + "]", i);

		if (textures[i].type() == "texture_specular")
			shader.uniform1i("material.texture_specular[" + std::to_string(specularTextureCount++) + "]", i);
	}

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
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glEnableVertexArrayAttrib(VAO, 2);
	glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribBinding(VAO, 2, 0);

	glVertexArrayElementBuffer(VAO, EBO);
}

void Mesh::release()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &EBO);
}

