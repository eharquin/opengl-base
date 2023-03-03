#pragma once
#include "openglbase.h"
#include "stb_image.h"

class Texture
{
public:
	Texture(const std::string& textureFilePath);
	~Texture();

	// delete copy constructor
	Texture(const Texture&) = delete;
	// delete move constructor
	Texture(Texture&& other) = delete;
	// delete assignment operators
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& other) = delete;

	GLuint id() const { return id_; }

	void bind(int unit);

private:
	GLuint id_;

};