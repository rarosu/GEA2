#include "ThreadPool.h"
#include <glew.h>
#include <assert.h>

extern SDL_Window* g_window;

Worker::Worker(size_t _id, ThreadPool& _pool, SDL_GLContext _context)
	: id(_id), pool(_pool), context(_context)
{}

void Worker::operator()()
{
	SDL_GL_MakeCurrent(g_window, context);

	TaskFunc task;
	void* data;
	while(true)
	{
		{
			std::unique_lock<std::mutex> lock(pool.mutex);
		
			while(pool.tasks.empty() && !pool.stop)
			{
				pool.cond.wait(lock);
			}

			if(pool.stop)
				return;

			task = pool.tasks.back();
			pool.tasks.pop_back();

			data = pool.data.back();
			pool.data.pop_back();
		}
		
		task(data, id);
	}

	SDL_GL_MakeCurrent(g_window, nullptr);
	SDL_GL_DeleteContext(context);
}

ThreadPool::ThreadPool(size_t size)
	: stop(false)
{
	threads.resize(size);
	std::vector<SDL_GLContext> contexts(size + 1);
	for(size_t i = 0; i < contexts.size(); i++)
	{
		contexts[i] = SDL_GL_CreateContext(g_window);
		assert(contexts[i] != nullptr);
	}
	for(size_t i = 0; i < threads.size(); i++)
	{
		threads[i] = std::thread(Worker(i, *this, contexts[i]));
	}
}

ThreadPool::~ThreadPool()
{
	stop = true;

	cond.notify_all();

	for(size_t i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}
}

void ThreadPool::AddTask(TaskFunc task, void* taskData)
{
	mutex.lock();

	tasks.push_back(task);
	data.push_back(taskData);

	mutex.unlock();

	cond.notify_one();
}