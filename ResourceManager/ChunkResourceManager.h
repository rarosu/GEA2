#pragma once

#include "Resource.h"
#include "ResourceContainer.h"
#include "Filesystem.h"
#include "MemoryAllocator.h"
#include "../GEA2/Renderer/Chunk/Chunk.h"
#include "Memory/PoolAllocator.h"

#include <mutex>
#define SCX 32
#define SCY 2
#define SCZ 32
class ChunkResourceManager
{
PUBLIC:
	ChunkResourceManager(Filesystem* filesystem, MemoryAllocator* allocator);
	~ChunkResourceManager();

	Resource<Chunk> Load(int x, int y, int z);
	void Destructor(InternalResource<Chunk>* internal);
PRIVATE:
	struct header_element
	{
		int address;
		int size;
	};

	header_element header[SCX * SCY * SCZ];
	ResourceContainer<Chunk> chunks;
	Filesystem* filesystem;
	MemoryAllocatorInterface allocator;
	void* chunkMem;
	ThreadedPoolAllocator* pool;
	std::mutex mutex;
	std::shared_ptr<File> file;
};