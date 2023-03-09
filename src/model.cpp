#include "model.h"

Model::Model(const std::string& path)
{
	loadModel(path);
}

void Model::Draw(const Shader& shader) const
{
	for (int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::loadModel(const std::string& path)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/') + 1 );

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertex.uv = glm::vec2(0.0f, 0.0f);
		if (mesh->mTextureCoords[0])
			vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), std::make_move_iterator(diffuseMaps.begin()), std::make_move_iterator(diffuseMaps.end()));
		
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), std::make_move_iterator(specularMaps.begin()), std::make_move_iterator(specularMaps.end()));
	}

	Mesh newMesh(std::move(vertices), std::move(indices), std::move(textures));
	
	return newMesh;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType textureType, std::string typeName)
{
	std::vector<Texture> textures;
	for (int i = 0; i < material->GetTextureCount(textureType); i++)
	{
		aiString path;
		material->GetTexture(textureType, i, &path);
		
		bool alreadyLoad = false;
		for (int j = 0; j < loadedTextures.size(); j++)
		{
			if (loadedTextures[j].path() == path.C_Str())
			{
				textures.push_back(std::move(loadedTextures[j]));
				alreadyLoad = true;
				break;
			}
		}
		if (!alreadyLoad)
		{
			Texture texture(directory + path.C_Str(), typeName);
			textures.push_back(std::move(texture));
			loadedTextures.push_back(std::move(texture));
		}
	}
	return textures;
}
