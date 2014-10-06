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

	InternalResource<Texture>* internal = textures.GetResource(hash);
	if (internal != nullptr)
		return Resource<Texture>(internal, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));

	char* filedata;
	size_t filesize;

	{

		std::shared_ptr<File> file = filesystem->GetFile(vpath);

		if (file == nullptr)
			return Resource<Texture>();

		filesize = file->GetFileSize();

		filedata = new char[filesize];
		file->Read(filedata, filesize);
	
	}

	
	SDL_Surface* surface;
	{
		std::lock_guard<std::mutex> lock(mutex);
		SDL_RWops* rw = SDL_RWFromConstMem(filedata, filesize);
		surface = IMG_LoadPNG_RW(rw);
	}
	
	delete[] filedata;

	Texture* texture = new Texture;
	texture->surface = surface;

	internal = textures.AddResource(hash, texture);
	if (internal == nullptr)
		return Resource<Texture>();

	return Resource<Texture>(internal, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));
}