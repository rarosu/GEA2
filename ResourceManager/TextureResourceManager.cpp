#include "TextureResourceManager.h"

TextureResourceManager::TextureResourceManager(Filesystem* filesystem)
{
	this->filesystem = filesystem;
}

void TextureResourceManager::Destructor(InternalResource<SDL_Surface>* internal)
{
	SDL_FreeSurface(internal->resource);
	textures.RemoveResource(internal->hash);
}

Resource<SDL_Surface> TextureResourceManager::Load(const std::string& vpath)
{
	std::hash<std::string> hasher;
	size_t hash = hasher(vpath);

	InternalResource<SDL_Surface>* internal = textures.GetResource(hash);
	if (internal != nullptr)
		return Resource<SDL_Surface>(internal, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));

	File* file = filesystem->GetFile(vpath);

	if (file == nullptr)
		return Resource<SDL_Surface>();

	if (!file->Open())
		return Resource<SDL_Surface>();

	char* filedata = new char[file->GetFileSize()];
	file->Read(filedata, file->GetFileSize());
	SDL_RWops* rw = SDL_RWFromConstMem(filedata, file->GetFileSize());
	SDL_Surface* surface = IMG_LoadPNG_RW(rw);
	delete[] filedata;

	if (!file->Close())
	{
		SDL_FreeSurface(surface);
		return Resource<SDL_Surface>();
	}

	internal = textures.AddResource(hash, surface);
	if (internal == nullptr)
		return Resource<SDL_Surface>();

	return Resource<SDL_Surface>(internal, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));
}