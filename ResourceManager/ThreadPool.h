#pragma once

#include <SDL.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

typedef void(*TaskFunc)(void* data, size_t id);

class ThreadPool;

class Worker
{
public:
	Worker(size_t _id, ThreadPool& _pool, SDL_GLContext _context);
	void operator()();
private:
	size_t id;
	ThreadPool& pool;
	SDL_GLContext context;
};

class ThreadPool
{
public:
	friend class Worker;
	ThreadPool(size_t size);
	~ThreadPool();

	void AddTask(TaskFunc task, void* taskData);

private:
	std::vector<std::thread> threads;
	std::vector<TaskFunc> tasks;
	std::vector<void*> data;
	std::mutex mutex;
	std::condition_variable cond;
	bool stop;
};