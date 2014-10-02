#define ALL_PUBLIC

#include <gtest/gtest.h>
#include <SDL.h>
#include <SDL_image.h>
#include <glew.h>
#include <iostream>

#include <ZipArchive.h>
#include <ThreadPool.h>
#include <Filesystem.h>
#include <TextureResourceManager.h>

SDL_Window* g_window;
TextureResourceManager* g_textureLoader;

class ThreadPoolTest : public ::testing::Test
{
public:
	void SetUp()
	{
		SDL_Init(SDL_INIT_TIMER);
		IMG_Init(IMG_INIT_PNG);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		g_window = SDL_CreateWindow("GEA2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_OPENGL);

		int glContextFlags = 0;
#if defined (_DEBUG) 
		glContextFlags = SDL_GL_CONTEXT_DEBUG_FLAG;
#endif 
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, glContextFlags);
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

		pool = new ThreadPool(8);

		g_textureLoader = new TextureResourceManager(&filesystem);

		// Initiate GLEW. 
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			printf("Error init glew.\n");
			return;
		}
	}

	void TearDown()
	{
		delete g_textureLoader;
		delete pool;

		SDL_DestroyWindow(g_window);
		IMG_Quit();
		SDL_Quit();
	}

protected:
	Filesystem filesystem;
	ThreadPool* pool;

};


struct SimpleTaskResult
{
	int value;
	GLuint buffer;
};

struct SimpleTask
{
	SimpleTaskResult operator()(int a, int b)
	{
		SimpleTaskResult result = { 0, 0 };
		result.value = a + b;
		glGenBuffers(1, &result.buffer);
		return result;
	}
};

std::mutex g_mutex;

struct LoadTextureTask
{
	Resource<Texture> operator()(const std::string& path)
	{
		Resource<Texture> texture = g_textureLoader->Load(path);

		g_mutex.lock();
		glGenTextures(1, &texture->texture);
		g_mutex.unlock();

		return texture;
	}
};

TEST_F(ThreadPoolTest, SimpleThreadPoolTest)
{
	/*
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);

		std::future<SimpleTaskResult> future = pool->AddTask<SimpleTask>(15, 15);

		future.wait();

		SimpleTaskResult result = future.get();

		ASSERT_EQ(result.value, 30);
		ASSERT_EQ(result.buffer, 2);
	}*/

	{
		filesystem.AddArchive<ZipArchive>("../assets/Assets.zip");

		//filesystem.AddArchive<ZipArchive>("../assets/TestTextures.zip");
		//filesystem.AddArchive<ZipArchive>("../assets/TestTextures2.zip");

		std::future<Resource<Texture>> future = pool->AddTask<LoadTextureTask>("TestTexture.png");
		std::future<Resource<Texture>> future2 = pool->AddTask<LoadTextureTask>("TestTexture2.png");

		Resource<Texture> r1 = future.get();
		ASSERT_NE(r1, nullptr);
		ASSERT_EQ(r1->surface->w, 1406);
		ASSERT_EQ(r1->surface->h, 681);

		Resource<Texture> r2 = future2.get();
		ASSERT_NE(r2, nullptr);
		ASSERT_EQ(r2->surface->w, 1406);
		ASSERT_EQ(r2->surface->h, 681);
	}

}




