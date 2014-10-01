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

		pool = new ThreadPool(10);

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

std::mutex m;

/*struct task_data
{
	GLuint buffer;
};

void* task(void* data, size_t id)
{
	task_data* td = (task_data*)data;
	glGenBuffers(1, &td->buffer);

	return nullptr;
}

TEST_F(ThreadPoolTest, SimpleThreadPoolTest)
{
	task_data data[100];
	memset(data, 0, sizeof(task_data) * 100);

	for(size_t i = 0; i < 32; i++)
	{
		pool->AddTask(&task, &data[i]);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	for(size_t i = 0; i < 32; i++)
	{
		ASSERT_NE(data[i].buffer, 0);
	}
}*/

struct TaskInData
{
	float scale;
};

struct TaskOutData
{
	GLuint buffer;
};

struct Task
{
	TaskOutData operator()(TaskInData in)
	{
		TaskOutData a;
		glGenBuffers(1, &a.buffer);
		return a;
	}
};

/*TaskOutData Func(TaskInData in)
{
	TaskOutData a;
	glGenBuffers(1, &a.buffer);
	return a;
}*/


TEST_F(ThreadPoolTest, AdvThreadPoolTest)
{
	static const int NrTasks = 32;

	std::vector<TaskInData> in(NrTasks);
	std::vector<std::future<TaskOutData>> results(NrTasks);

	// Add Tasks with unique in-data.
	for(size_t i = 0; i < NrTasks; i++)
	{
		results[i] = pool->AddTask<Task>(std::move(in[i]));
	}

	while(!results.empty())
	{
		auto r = std::move(results.back());
		results.pop_back();

		r.wait();
		ASSERT_NE(r.get().buffer, 0);
	}
}