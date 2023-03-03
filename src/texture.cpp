#include "texture.h"

Texture::Texture(const std::string& textureFilePath)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &id_);

	// define texture wrapping (S,T like X,Y)
	glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set magnifying and minifying operations (when scaling up or downwards the image)
	glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// use stb lib to read the image
	stbi_set_flip_vertically_on_load(true);
	int widthTexture, heightTexture, numColCh;
	unsigned char* data = stbi_load(textureFilePath.c_str(), &widthTexture, &heightTexture, &numColCh, 0);
	if (!data)
	{
		throw std::runtime_error("Failed to load texture");
	}

	// submit image information to the texture
	glTextureStorage2D(id_, 1, GL_RGBA8, widthTexture, heightTexture);
	glTextureSubImage2D(id_, 0, 0, 0, widthTexture, heightTexture, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// generate mipmap (ref to minifying operation)
	glGenerateTextureMipmap(id_);

	stbi_image_free(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id_);

}

void Texture::bind(int unit)
{
	glBindTextureUnit(unit, id_);
}
