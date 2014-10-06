#include "PoolAllocator.h"
#include <malloc.h>
#include <cassert>

PoolAllocator::PoolAllocator(unsigned elementSize, unsigned numElements)
	: m_start(nullptr), m_next(nullptr)
{
	m_start = (PoolElement*)malloc( elementSize * numElements );

	Initialize(elementSize, numElements);
}

PoolAllocator::~PoolAllocator()
{
    if(m_start != 0) {
        free(m_start);
        m_start = 0;
    }
}

void PoolAllocator::Initialize(unsigned elementSize, unsigned numElements)
{
	union 
	{
		void* as_void;
		char* as_char;
		PoolElement* as_self;
	};

	as_void = m_start;
	m_next = as_self;

	PoolElement* runner = m_next;
	for(unsigned i = 0; i < numElements; ++i)
	{	
		runner->m_next = as_self;
		runner = as_self;
		as_char += elementSize;
	}

	runner->m_next = nullptr;
};

void PoolAllocator::Free()
{
    free(m_start);
}

void* PoolAllocator::Alloc()
{
	assert(m_next != nullptr);
		
	PoolElement* head = m_next;
	m_next = head->m_next;
	return head;

	/*
	// Reached the end of list return nullptr.
	if (m_next == nullptr) {
		return nullptr;
	}
 
	PoolElement* head = m_next;
	m_next = head->m_next;
	return head;
	*/
}
 
void PoolAllocator::Free(void* ptr)
{
	//PoolElement* head = static_cast<PoolElement*>(ptr);
	PoolElement* head = (PoolElement*)ptr;
	head->m_next = m_next;
	m_next = head;
}

ThreadedPoolAllocator::ThreadedPoolAllocator(unsigned elementSize, unsigned numElements)
	: allocator(elementSize, numElements)
{

}

void* ThreadedPoolAllocator::Alloc()
{
	std::lock_guard<std::mutex> lock(mtx);
	return allocator.Alloc();
}
 
void ThreadedPoolAllocator::Free(void* ptr)
{
	std::lock_guard<std::mutex> lock(mtx);
	allocator.Free(ptr);
}


DefaultMemoryManager::DefaultMemoryManager(unsigned elementSize)
{
	this->elementSize = elementSize;
}

void* DefaultMemoryManager::Alloc()
{
	return malloc(elementSize);
}

void DefaultMemoryManager::Free(void* ptr)
{
	free(ptr);
}

