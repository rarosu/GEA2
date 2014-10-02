#define ALL_PUBLIC

#include <gtest/gtest.h>
#include <SDL.h>
#include <glew.h>
#include <ThreadPool.h>
#include <iostream>

SDL_Window* g_window;

class ThreadPoolTest : public ::testing::Test
{
public:
	ThreadPoolTest()
	{
		SDL_Init(SDL_INIT_TIMER);

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

		pool = new ThreadPool(1);

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
		delete pool;

		SDL_DestroyWindow(g_window);
		SDL_Quit();
	}

protected:
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