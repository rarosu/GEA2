#include "MemoryAllocator.h"

MemoryAllocator::MemoryAllocator(std::ostream& _logger)
	: logger(_logger) {}

MemoryAllocatorInterface MemoryAllocator::GetInterface(const std::string& name)
{
	return MemoryAllocatorInterface(name, this);
}

void* MemoryAllocator::Alloc(const MemoryAllocatorInterface& interface, size_t bytecount)
{
	void* ptr = new char[bytecount];

	logger << interface.GetName() << " allocated " << bytecount << " (" << ptr << ")" << std::endl;
	
	return ptr;
}

void MemoryAllocator::Free(const MemoryAllocatorInterface& interface, void* ptr)
{
	logger << interface.GetName() << " deleted " << ptr << std::endl;

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