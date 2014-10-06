#include "TextureResourceManager.h"

TextureResourceManager::TextureResourceManager(Filesystem* filesystem, MemoryAllocator* _allocator)
	: allocator(_allocator->GetInterface("TextureResourceManager"))
{
	this->filesystem = filesystem;
}

TextureResourceManager::~TextureResourceManager()
{
	
}

void TextureResourceManager::Destructor(InternalResource<Texture>* internal)
{
	allocator.Free(internal->resource);
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

		filedata = (char*) allocator.Alloc(filesize);
		file->Read(filedata, filesize);
	
	}

	
	SDL_Surface* surface;
	{
		std::lock_guard<std::mutex> lock(mutex);
		SDL_RWops* rw = SDL_RWFromConstMem(filedata, filesize);
		surface = IMG_LoadPNG_RW(rw);
	}
	
	allocator.Free(filedata);

	//Texture* texture = new Texture;
	Texture* texture = (Texture*) allocator.Alloc(sizeof(Texture));
	texture->surface = surface;

	internal = textures.AddResource(hash, texture);
	if (internal == nullptr)
		return Resource<Texture>();

	return Resource<Texture>(internal, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));
}