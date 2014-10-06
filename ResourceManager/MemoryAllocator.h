#pragma once

#include <iostream>
#include <string>
#include <map>
#include <mutex>

class MemoryAllocator;
class MemoryAllocatorInterface;


class MemoryAllocator
{
	friend class MemoryAllocatorInterface;
public:
	~MemoryAllocator();

	MemoryAllocatorInterface GetInterface(const std::string& name);
private:
	struct AllocationInfo
	{
		size_t size;
		std::string name;
	};

	std::mutex mutex;
	std::map<void*, AllocationInfo> allocations;

	void* Alloc(const MemoryAllocatorInterface& interface, size_t bytecount);
	void Free(const MemoryAllocatorInterface& interface, void* ptr);
};


class MemoryAllocatorInterface
{
	friend class MemoryAllocator;
public:
	void* Alloc(size_t bytecount);
	void Free(void* ptr);

	const std::string& GetName() const;
private:
	std::string name;
	MemoryAllocator* allocator;

	MemoryAllocatorInterface(const std::string& name, MemoryAllocator* allocator);
};