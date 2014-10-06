#pragma once

#include <mutex>

class StackAllocator
{
public:
	StackAllocator(unsigned int stackSize_bytes);
	~StackAllocator();

	void* Alloc(unsigned int size_bytes);
	void Clear();

	unsigned int GetTotalSize() const;
	unsigned int GetAllocatedSize() const;

private:
	void* m_mem;
	void* m_ptr;
	unsigned int m_stackSize_bytes;
};

class StackMemoryManager
{
public:
	StackMemoryManager(unsigned int stackSize_bytes);

	void* Alloc(unsigned int size_bytes);
	void Clear();
private:
	StackAllocator allocator;
	std::mutex mtx;
};

inline void* operator new(size_t nbytes, StackMemoryManager& manager)
{
	return manager.Alloc(nbytes);
}