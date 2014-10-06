#pragma once

#include "Resource.h"
#include "ResourceContainer.h"
#include "Filesystem.h"
#include "../GEA2/Renderer/Chunk/Chunk.h"

#include <mutex>
#define SCX 256
#define SCY 2
#define SCZ 256
class ChunkResourceManager
{
PUBLIC:
	ChunkResourceManager(Filesystem* filesystem);
	~ChunkResourceManager();

	Resource<Chunk> Load(int x, int y, int z);
	void Destructor(InternalResource<Chunk>* internal);
PRIVATE:
	ResourceContainer<Chunk> chunks;
	Filesystem* filesystem;
	std::mutex mutex;

};