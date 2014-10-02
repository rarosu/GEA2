#include "TextureResourceManager.h"

TextureResourceManager::TextureResourceManager(Filesystem* filesystem)
{
	this->filesystem = filesystem;
}

void TextureResourceManager::Destructor(const InternalResource<SDL_Surface>::Iterator& iterator)
{
	SDL_FreeSurface(iterator->second.resource);
	textures.RemoveResource(iterator);
}

Resource<SDL_Surface> TextureResourceManager::Load(const std::string& vpath)
{
	std::hash<std::string> hasher;
	size_t hash = hasher(vpath);

	InternalResource<SDL_Surface>::Iterator internal = textures.GetResource(hash);
	if (internal != textures.GetEnd())
		return Resource<SDL_Surface>(internal, &textures, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));

	File* file = filesystem->GetFile(vpath);

	if (file == nullptr)
		return Resource<SDL_Surface>::GetEmptyResource(&textures);

	if (!file->Open())
		return Resource<SDL_Surface>::GetEmptyResource(&textures);

	char* filedata = new char[file->GetFileSize()];
	file->Read(filedata, file->GetFileSize());
	SDL_RWops* rw = SDL_RWFromConstMem(filedata, file->GetFileSize());
	SDL_Surface* surface = IMG_LoadPNG_RW(rw);
	delete[] filedata;

	if (!file->Close())
	{
		SDL_FreeSurface(surface);
		return Resource<SDL_Surface>::GetEmptyResource(&textures);
	}

	internal = textures.AddResource(hash, surface);
	return Resource<SDL_Surface>(internal, &textures, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));
}