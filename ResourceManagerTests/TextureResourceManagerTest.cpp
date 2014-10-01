#define ALL_PUBLIC

#include <gtest/gtest.h>
#include <SDL.h>
#include <SDL_image.h>
#include <Resource.h>
#include <ResourceContainer.h>
#include <ZipArchive.h>
#include <Filesystem.h>
#include <functional>

class TextureResourceManager
{
PUBLIC:
	TextureResourceManager(Filesystem* filesystem);

	Resource<SDL_Surface> Load(const std::string& vpath);
	void Destructor(const InternalResource<SDL_Surface>::Iterator& iterator);
PRIVATE:
	ResourceContainer<SDL_Surface> textures;
	Filesystem* filesystem;
};

class TextureResourceManagerTest : public ::testing::Test
{
public:
	TextureResourceManagerTest() : rm(&filesystem) 
	{
		SDL_Init(0);
		IMG_Init(IMG_INIT_PNG);
	}

	~TextureResourceManagerTest()
	{
		IMG_Quit();
		SDL_Quit();
	}
protected:
	Filesystem filesystem;
	TextureResourceManager rm;
};





TextureResourceManager::TextureResourceManager(Filesystem* filesystem)
{
	this->filesystem = filesystem;
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
	delete [] filedata;

	if (!file->Close())
	{
		SDL_FreeSurface(surface);
		return Resource<SDL_Surface>::GetEmptyResource(&textures);
	}
	
	internal = textures.AddResource(hash, surface);
	return Resource<SDL_Surface>(internal, &textures, std::bind(&TextureResourceManager::Destructor, this, std::placeholders::_1));
}

void TextureResourceManager::Destructor(const InternalResource<SDL_Surface>::Iterator& iterator)
{
	SDL_FreeSurface(iterator->second.resource);
	textures.RemoveResource(iterator);
}



TEST_F(TextureResourceManagerTest, TextureLoad)
{
	filesystem.AddArchive<ZipArchive>("../assets/TestTextures.zip");

	{
		Resource<SDL_Surface> r1 = rm.Load("TestTexture.png");
		ASSERT_FALSE(r1.IsEmpty());

		ASSERT_EQ(r1->w, 1406);
		ASSERT_EQ(r1->h, 681);

		Resource<SDL_Surface> r3 = rm.Load("TestTexture.png");
		ASSERT_EQ(r1, r3);
	}

	ASSERT_EQ(rm.textures.resources.size(), 0);

	Resource<SDL_Surface> r2 = rm.Load("NotExisting.png");
	ASSERT_TRUE(r2.IsEmpty());
}