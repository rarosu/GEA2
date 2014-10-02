#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "Resource.h"
#include "ResourceContainer.h"
#include "Filesystem.h"

#include <glew.h>
#include <mutex>

struct Texture
{
	~Texture()
	{
		SDL_FreeSurface(surface);
	}

	SDL_Surface* surface;
	GLuint texture;
};

class TextureResourceManager
{
PUBLIC:
	TextureResourceManager(Filesystem* filesystem);
	~TextureResourceManager();

	Resource<Texture> Load(const std::string& vpath);
	void Destructor(InternalResource<Texture>* internal);
PRIVATE:
	ResourceContainer<Texture> textures;
	Filesystem* filesystem;
	std::mutex mutex;
};