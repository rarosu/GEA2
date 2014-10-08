#pragma once

#include "Resource.h"
#include "ResourceContainer.h"
#include "Filesystem.h"
#include "../GEA2/Renderer/Chunk/Chunk.h"

#include <mutex>

class ChunkResourceManager
{
PUBLIC:

	struct header_global
	{
		int header_size;
		int SCX;
		int SCY;
		int SCZ;
		int CX;
		int CY;
		int CZ;
	};

	ChunkResourceManager(Filesystem* filesystem);
	~ChunkResourceManager();

	Resource<Chunk> Load(int x, int y, int z);
	void Destructor(InternalResource<Chunk>* internal);

	const header_global globalFileHeader;
	
PRIVATE:

	struct header_element
	{
		int address;
		int size;
	};

	header_element* header;
	ResourceContainer<Chunk> chunks;
	Filesystem* filesystem;
	std::mutex mutex;
	std::shared_ptr<File> file;
};