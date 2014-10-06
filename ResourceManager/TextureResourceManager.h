#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "Resource.h"
#include "ResourceContainer.h"
#include "Filesystem.h"

class TextureResourceManager
{
PUBLIC:
	TextureResourceManager(Filesystem* filesystem);

	Resource<SDL_Surface> Load(const std::string& vpath);
	void Destructor(InternalResource<SDL_Surface>* internal);
PRIVATE:
	ResourceContainer<SDL_Surface> textures;
	Filesystem* filesystem;
};