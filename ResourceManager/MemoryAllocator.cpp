#include "MemoryAllocator.h"

MemoryAllocator::~MemoryAllocator()
{
	std::cout << "-- Unallocated Memory --" << std::endl;
	for (auto it : allocations)
	{
		std::cout << "[" << it.second.name << "] " << it.first << " with size " << it.second.size << std::endl;
	}
}

MemoryAllocatorInterface MemoryAllocator::GetInterface(const std::string& name)
{
	return MemoryAllocatorInterface(name, this);
}

void* MemoryAllocator::Alloc(const MemoryAllocatorInterface& interface, size_t bytecount)
{
	void* ptr = new char[bytecount];

	{
		std::lock_guard<std::mutex> lock(mutex);
		allocations[ptr] = { bytecount, interface.GetName() };
	}
	
	return ptr;
}

void MemoryAllocator::Free(const MemoryAllocatorInterface& interface, void* ptr)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		allocations.erase(ptr);
	}

	delete[] ptr;
}

MemoryAllocatorInterface::MemoryAllocatorInterface(const std::string& name, MemoryAllocator* allocator)
{
	this->name = name;
	this->allocator = allocator;
}

void* MemoryAllocatorInterface::Alloc(size_t bytecount)
{
	return allocator->Alloc(*this, bytecount);
}

void MemoryAllocatorInterface::Free(void* ptr)
{
	return allocator->Free(*this, ptr);
}

const std::string& MemoryAllocatorInterface::GetName() const
{
	return name;
}