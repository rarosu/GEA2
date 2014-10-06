#define ALL_PUBLIC

#include <gtest/gtest.h>
#include <ZipArchive.h>
#include <functional>
#include <TextureResourceManager.h>

class TextureResourceManagerTest : public ::testing::Test
{
public:
	TextureResourceManagerTest() : rm(&filesystem, &allocator)
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
	MemoryAllocator allocator;
	Filesystem filesystem;
	TextureResourceManager rm;
};

TEST_F(TextureResourceManagerTest, TextureLoad)
{
	filesystem.AddArchive<ZipArchive>("../assets/TestTextures.zip");

	{
		Resource<Texture> r1 = rm.Load("TestTexture.png");
		ASSERT_NE(r1, nullptr);

		ASSERT_EQ(r1->surface->w, 1406);
		ASSERT_EQ(r1->surface->h, 681);

		Resource<Texture> r3 = rm.Load("TestTexture.png");
		ASSERT_EQ(r1, r3);
	}

	ASSERT_EQ(rm.textures.resources.size(), 0);

	Resource<Texture> r2 = rm.Load("NotExisting.png");
	ASSERT_EQ(r2, nullptr);
}