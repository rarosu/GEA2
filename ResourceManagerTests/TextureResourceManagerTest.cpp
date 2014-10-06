#define ALL_PUBLIC

#include <gtest/gtest.h>
#include <ZipArchive.h>
#include <functional>
#include <TextureResourceManager.h>

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

TEST_F(TextureResourceManagerTest, TextureLoad)
{
	filesystem.AddArchive<ZipArchive>("../assets/TestTextures.zip");

	{
		Resource<SDL_Surface> r1 = rm.Load("TestTexture.png");
		ASSERT_NE(r1, nullptr);

		ASSERT_EQ(r1->w, 1406);
		ASSERT_EQ(r1->h, 681);

		Resource<SDL_Surface> r3 = rm.Load("TestTexture.png");
		ASSERT_EQ(r1, r3);
	}

	ASSERT_EQ(rm.textures.resources.size(), 0);

	Resource<SDL_Surface> r2 = rm.Load("NotExisting.png");
	ASSERT_EQ(r2, nullptr);
}