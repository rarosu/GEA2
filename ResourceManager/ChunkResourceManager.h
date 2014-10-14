#pragma once

#include "Resource.h"
#include "ResourceContainer.h"
#include "Filesystem.h"
#include "MemoryAllocator.h"
#include "../GEA2/Renderer/Chunk/Chunk.h"
#include "Memory/PoolAllocator.h"
#include <mutex>

struct MetaWorldHeader
{
	int header_size;
	int SCX;
	int SCY;
	int SCZ;
	int CX;
	int CY;
	int CZ;
};
const int MAX_CHUNKS_IN_MEM = 128 * 4 * 128;

class ChunkResourceManager
{
PUBLIC:

	ChunkResourceManager(Filesystem* filesystem, MemoryAllocator* allocator, const std::string& vWorldPath);
	~ChunkResourceManager();

	Resource<Chunk> Get(int x, int y, int z);
	Resource<Chunk> Load(int x, int y, int z);
	void Destructor(InternalResource<Chunk>* internal);
	const MetaWorldHeader& GetGlobalWorldHeader();
	
PRIVATE:

	struct header_element
	{
		int address;
		int size;
	};

	header_element* header;
	ResourceContainer<Chunk> chunks;
	Filesystem* filesystem;
	MemoryAllocatorInterface allocator;
	void* chunkMem;
	ThreadedPoolAllocator* pool;
	std::mutex mutex;
	std::shared_ptr<File> file;
	MetaWorldHeader globalFileHeader;

	std::string currentVWorldPath;
};