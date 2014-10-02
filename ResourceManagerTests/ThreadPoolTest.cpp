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
	ThreadPoolTest()
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

	~ThreadPoolTest()
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

#pragma region SimpleThreadPoolTest
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

TEST_F(ThreadPoolTest, SimpleThreadPoolTest)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);

	std::future<SimpleTaskResult> future = pool->AddTask<SimpleTask>(15, 15);
	
	future.wait();

	SimpleTaskResult result = future.get();

	ASSERT_EQ(result.value, 30);
	ASSERT_EQ(result.buffer, 2);
}
#pragma endregion


struct LoadTextureTask
{
	Resource<SDL_Surface> operator()(const std::string& path)
	{
		return g_textureLoader->Load(path);
	}
};

/*
TEST_F(ThreadPoolTest, TextureThreadPoolTest)
{
	filesystem.AddArchive<ZipArchive>("../assets/TestTextures.zip");

	std::future<Resource<SDL_Surface>> future = pool->AddTask<LoadTextureTask>("TestTexture.png");

	future.wait();

	Resource<SDL_Surface> r1 = future.get();

	ASSERT_FALSE(r1.IsEmpty());
	ASSERT_EQ(r1->w, 1406);
	ASSERT_EQ(r1->h, 681);
}
*/