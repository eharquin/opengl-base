#pragma once
#include "openglbase.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	Model(const std::string& path);

	// delete copy constructor
	Model(const Model&) = delete;
	// delete move constructor
	Model(Model&& other) = delete;
	// delete assignment operators
	Model& operator=(const Model&) = delete;
	Model& operator=(Model&& other) = delete;

	void Draw(const Shader& shader) const;

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> loadedTextures;
	std::string directory;

	void loadModel(const std::string& path);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType textureType, std::string typeName);


};
