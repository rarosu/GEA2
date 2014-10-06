#pragma once

#include <glew.h>
#include <SDL_image.h>

class Texture
{
public:
	Texture();
	~Texture();

	void Load(const char* filePath);
	void Bind(unsigned slot);
	void Unbind(unsigned slot);

private:

	GLuint textureHandle;
};