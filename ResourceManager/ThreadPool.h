#pragma once

#include <SDL.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <functional>
#include <future>
#include <memory>
#include <queue>

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

	template<typename T, typename... Args>
	std::future<typename std::result_of<T(Args...)>::type> AddTask(Args&&... args);

private:
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::mutex mutex;
	std::condition_variable cond;
	bool stop;
};

template<typename T, typename... Args>
std::future<typename std::result_of<T(Args...)>::type> ThreadPool::AddTask(Args&&... args)
{
	typedef typename std::result_of<T(Args...)>::type return_type;
	
	auto ptr = std::make_shared<std::packaged_task<return_type()>>(std::bind(T(), std::forward<Args>(args)...));

	std::future<return_type> future = ptr->get_future();

	{
		std::unique_lock<std::mutex> lock(mutex);
		tasks.push([ptr](){ (*ptr)(); });
	}

	cond.notify_one();

	return future;
}