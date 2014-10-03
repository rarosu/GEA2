#include "Texture.h"
#include <iostream>

Texture::Texture()
{
	glGenTextures(1, &textureHandle);
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureHandle);
}

void Texture::Load(const char* filePath)
{
	SDL_Surface* surf = IMG_Load(filePath);
	if (!surf)
	{
		std::cout << "Texture load failed!" << std::endl;
		return;
	}
	 
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if (surf->format->BytesPerPixel == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, surf->w, surf->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surf->pixels);
	else if (surf->format->BytesPerPixel == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
	else
		std::cout << "Texture format not supported!" << std::endl;

	SDL_FreeSurface(surf);
}

void Texture::Bind(unsigned slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}

void Texture::Unbind(unsigned slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}

