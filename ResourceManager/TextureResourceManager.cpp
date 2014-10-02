#include "TextureResourceManager.h"

TextureResourceManager::TextureResourceManager(Filesystem* filesystem)
{
	this->filesystem = filesystem;
}

TextureResourceManager::~TextureResourceManager()
{
	
}

void TextureResourceManager::Destructor(InternalResource<Texture>* internal)
{
	textures.RemoveResource(internal->hash);
}

Resource<Texture> TextureResourceManager::Load(const std::string& vpath)
{
	std::hash<std::string> hasher;
	size_t hash = hasher(vpath);

	std::lock_guard<std::mutex> lock(mutex);

	InternalResource<Texture>* internal = textures.GetResource(hash);
	if (internal != nullptr)
		return Resource<Texture>(internal, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));

	File* file = filesystem->GetFile(vpath);

	if (file == nullptr)
		return Resource<Texture>();

	if (!file->Open())
		return Resource<Texture>();

	char* filedata = new char[file->GetFileSize()];
	file->Read(filedata, file->GetFileSize());
	SDL_RWops* rw = SDL_RWFromConstMem(filedata, file->GetFileSize());
	SDL_Surface* surface = IMG_LoadPNG_RW(rw);
	delete[] filedata;

	Texture* texture = new Texture;
	texture->surface = surface;

	if (!file->Close())
	{
		SDL_FreeSurface(surface);
		return Resource<Texture>();
	}
	
	internal = textures.AddResource(hash, texture);
	if (internal == nullptr)
		return Resource<Texture>();

	return Resource<Texture>(internal, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));
}