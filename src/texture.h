#pragma once
#include "openglbase.h"
#include "stb_image.h"

class Texture
{
public:
	Texture(const std::string& path, const std::string& type);
	~Texture();

	// delete the copy constructor/assignment
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// define move constructor
	Texture(Texture&& other);
	// define move assignement operator
	Texture& operator=(Texture&& other);

	GLuint id() const { return id_; }
	std::string type() const { return type_; }
	std::string path() const { return path_; }

	void bind(int unit) const;

private:
	GLuint id_ = GL_NONE;
	std::string type_; // "texture_specular" or "texture_diffuse"
	std::string path_;

	void release();
};