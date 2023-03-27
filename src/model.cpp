#include "model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Model::Model(const std::string& path)
{
	loadModel(path);
}

Model::Model(const Geometry& geometry, const Material& material)
{
	Mesh mesh(std::move(geometry.getVertices()), std::move(geometry.getIndices()), 0);
	meshes.push_back(std::move(mesh));

	materials.push_back(material);
}

void Model::Draw(const Shader& shader) const
{
	for (int i = 0; i < loadedTextures.size(); i++)
		loadedTextures[i].bind(i);


	for (int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader, materials[meshes[i].materialId]);
}

void Model::loadModel(const std::string& path)
{
	directory = path.substr(0, path.find_last_of('/') + 1);

	tinyobj::ObjReaderConfig readerConfig;
	readerConfig.mtl_search_path = "";

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(path, readerConfig)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		exit(1);
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	const tinyobj::attrib_t& attrib = reader.GetAttrib();
	const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
	const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();


	// loop over materials
	for (size_t materialCount = 0; materialCount < materials.size(); materialCount++)
	{
		int ambientTextureId = -1;

		if (materials[materialCount].ambient_texname != "")
		{
			bool ambientTextureAlreadyLoad = false;
			for (int j = 0; j < loadedTextures.size(); j++)
			{
				if (loadedTextures[j].path() == directory + materials[materialCount].ambient_texname)
				{
					ambientTextureId = j;
					ambientTextureAlreadyLoad = true;
					break;
				}
			}
			if (!ambientTextureAlreadyLoad)
			{
				Texture texture(directory + materials[materialCount].ambient_texname, " ");
				loadedTextures.push_back(std::move(texture));
				ambientTextureId = loadedTextures.size() - 1;
			}
		}

		int diffuseTextureId = -1;

		if (materials[materialCount].diffuse_texname != "")
		{
			bool diffuseTextureAlreadyLoad = false;
			for (int j = 0; j < loadedTextures.size(); j++)
			{
				if (loadedTextures[j].path() == directory + materials[materialCount].diffuse_texname)
				{
					diffuseTextureId = j;
					diffuseTextureAlreadyLoad = true;
					break;
				}
			}
			if (!diffuseTextureAlreadyLoad)
			{
				Texture texture(directory + materials[materialCount].diffuse_texname, " ");
				loadedTextures.push_back(std::move(texture));
				diffuseTextureId = loadedTextures.size() - 1;
			}
		}

		int specularTextureId = -1;

		if (materials[materialCount].specular_texname != "")
		{
			bool specularTextureAlreadyLoad = false;
			for (int j = 0; j < loadedTextures.size(); j++)
			{
				if (loadedTextures[j].path() == directory + materials[materialCount].specular_texname)
				{
					specularTextureId = j;
					specularTextureAlreadyLoad = true;
					break;
				}
			}
			if (!specularTextureAlreadyLoad)
			{
				Texture texture(directory + materials[materialCount].specular_texname, " ");
				loadedTextures.push_back(std::move(texture));
				specularTextureId = loadedTextures.size() - 1;
			}
		}

		glm::vec3 diffuse = glm::vec3(materials[materialCount].diffuse[0], materials[materialCount].diffuse[1], materials[materialCount].diffuse[2]);
		glm::vec3 ambient = glm::vec3(materials[materialCount].ambient[0], materials[materialCount].ambient[1], materials[materialCount].ambient[2]);
		float shininess = materials[materialCount].shininess;

		Material material(materials[materialCount].name, ambientTextureId, diffuseTextureId, specularTextureId, ambient, diffuse, shininess);
		this->materials.push_back(material);
	}

	// loop over shapes (meshes)
	for (size_t shapeCount = 0; shapeCount < shapes.size(); shapeCount++)
	{
		tinyobj::mesh_t mesh = shapes[shapeCount].mesh;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		std::unordered_map<Vertex, unsigned int> uniqueVertices;

		// loop over faces
		size_t indexOffset = 0;
		for (size_t faceCount = 0; faceCount < mesh.num_face_vertices.size(); faceCount++)
		{
			size_t numVertices = size_t(mesh.num_face_vertices[faceCount]);

			// loop over vertices in the face
			for (size_t vertexCount = 0; vertexCount < numVertices; vertexCount++)
			{
				Vertex vertex;

				// access to vertex
				tinyobj::index_t index = mesh.indices[indexOffset + vertexCount];
				tinyobj::real_t vertexX = attrib.vertices[3 * size_t(index.vertex_index) + 0];
				tinyobj::real_t vertexY = attrib.vertices[3 * size_t(index.vertex_index) + 1];
				tinyobj::real_t vertexZ = attrib.vertices[3 * size_t(index.vertex_index) + 2];

				vertex.position = glm::vec3(vertexX, vertexY, vertexZ);

				// check if `normal_index` is zero or positive. negative = no normal data
				if (index.normal_index >= 0)
				{
					tinyobj::real_t normalX = attrib.normals[3 * size_t(index.normal_index) + 0];
					tinyobj::real_t normalY = attrib.normals[3 * size_t(index.normal_index) + 1];
					tinyobj::real_t normalZ = attrib.normals[3 * size_t(index.normal_index) + 2];

					vertex.normal = glm::vec3(normalX, normalY, normalZ);
				}

				// check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (index.texcoord_index >= 0)
				{
					tinyobj::real_t texcoordX = attrib.texcoords[2 * size_t(index.texcoord_index) + 0];
					tinyobj::real_t texcoordY = attrib.texcoords[2 * size_t(index.texcoord_index) + 1];

					vertex.uv = glm::vec2(texcoordX, texcoordY);
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);

			}
			indexOffset += numVertices;

		}
		Mesh newMesh(std::move(vertices), std::move(indices), 0);
		meshes.push_back(std::move(newMesh));
	}
}


//void Model::loadModel(const std::string& path)
//{
//	Assimp::Importer importer;
//
//	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//	{
//		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
//		return;
//	}
//
//	directory = path.substr(0, path.find_last_of('/') + 1 );
//
//	processNode(scene->mRootNode, scene);
//}
//
//void Model::processNode(aiNode* node, const aiScene* scene)
//{
//	for (int i = 0; i < node->mNumChildren; i++)
//	{
//		processNode(node->mChildren[i], scene);
//	}
//
//	for (int i = 0; i < node->mNumMeshes; i++)
//	{
//		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//		meshes.push_back(processMesh(mesh, scene));
//	}
//}
//
//Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
//{
//	std::vector<Vertex> vertices;
//	std::vector<unsigned int> indices;
//	std::vector<Texture> textures;
//
//	for (int i = 0; i < mesh->mNumVertices; i++)
//	{
//		Vertex vertex;
//		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
//		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
//		vertex.uv = glm::vec2(0.0f, 0.0f);
//		if (mesh->mTextureCoords[0])
//			vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
//
//		vertices.push_back(vertex);
//	}
//
//	for (int i = 0; i < mesh->mNumFaces; i++)
//	{
//		aiFace face = mesh->mFaces[i];
//		for (int j = 0; j < face.mNumIndices; j++)
//			indices.push_back(face.mIndices[j]);
//	}
//
//	if (mesh->mMaterialIndex >= 0)
//	{
//		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//
//		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//		textures.insert(textures.end(), std::make_move_iterator(diffuseMaps.begin()), std::make_move_iterator(diffuseMaps.end()));
//		
//		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//		textures.insert(textures.end(), std::make_move_iterator(specularMaps.begin()), std::make_move_iterator(specularMaps.end()));
//	}
//
//	Mesh newMesh(std::move(vertices), std::move(indices), std::move(textures));
//	
//	return newMesh;
//}
//
//std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType textureType, std::string typeName)
//{
//	std::vector<Texture> textures;
//	for (int i = 0; i < material->GetTextureCount(textureType); i++)
//	{
//		aiString path;
//		material->GetTexture(textureType, i, &path);
//		
//		bool alreadyLoad = false;
//		for (int j = 0; j < loadedTextures.size(); j++)
//		{
//			if (loadedTextures[j].path() == path.C_Str())
//			{
//				textures.push_back(std::move(loadedTextures[j]));
//				alreadyLoad = true;
//				break;
//			}
//		}
//		if (!alreadyLoad)
//		{
//			Texture texture(directory + path.C_Str(), typeName);
//			textures.push_back(std::move(texture));
//			loadedTextures.push_back(std::move(texture));
//		}
//	}
//	return textures;
//}
