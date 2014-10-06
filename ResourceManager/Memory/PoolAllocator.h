#pragma once

#include <mutex>

struct PoolElement
{
	PoolElement* m_next;
};

class PoolAllocator
{
public:
	PoolAllocator(unsigned elementSize, unsigned numElements);
	~PoolAllocator();

	void Free();
	void* Alloc();
	void Free(void* ptr);

private:
	void Initialize(unsigned elementSize, unsigned numElements);

	PoolElement* m_start;
	PoolElement* m_next;
};

class ThreadedPoolAllocator
{
public:
	ThreadedPoolAllocator(unsigned elementSize, unsigned numElements);
	void* Alloc();
	void Free(void* ptr);
private:
	std::mutex mtx;
	PoolAllocator allocator;
};

class DefaultMemoryManager 
{
public:
	DefaultMemoryManager(unsigned elementSize);
	void* Alloc();
	void Free(void* ptr);
private:
	unsigned elementSize;
};