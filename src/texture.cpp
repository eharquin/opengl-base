#include "texture.h"

Texture::Texture(const std::string& path, const std::string& type)
	: type_(type), path_(path)
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
	unsigned char* data = stbi_load(path.c_str(), &widthTexture, &heightTexture, &numColCh, 0);
	if (!data)
	{
		throw std::runtime_error("Failed to load texture");
	}

	GLenum format;
	if (numColCh == 1)
	{
		format = GL_RED;
	}
	else if (numColCh == 3)
	{
		format = GL_RGB;
	}
	else if (numColCh == 4)
	{
		format = GL_RGBA;
	}

	// submit image information to the texture
	glTextureStorage2D(id_, 1, GL_RGBA8, widthTexture, heightTexture);
	glTextureSubImage2D(id_, 0, 0, 0, widthTexture, heightTexture, format, GL_UNSIGNED_BYTE, data);

	// generate mipmap (ref to minifying operation)
	glGenerateTextureMipmap(id_);

	stbi_image_free(data);
}

Texture::~Texture()
{
	release();
}

Texture::Texture(Texture&& other) : id_(other.id_), type_(other.type_), path_(other.path_)
{
	other.id_ = GL_NONE;
	other.type_ = "";
	other.path_ = "";
}

Texture& Texture::operator=(Texture&& other)
{
	if (this != &other)
	{
		release();
		std::swap(id_, other.id_);
		type_ = other.type_;
		path_ = other.path_;
	}

	return *this;
}

void Texture::bind(int unit) const
{
	glBindTextureUnit(unit, id_);
}

void Texture::release()
{
	glDeleteTextures(1, &id_); // it's ok to delete null texture
	id_ = GL_NONE;
}