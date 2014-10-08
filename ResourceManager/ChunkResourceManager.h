#pragma once

#include "Resource.h"
#include "ResourceContainer.h"
#include "Filesystem.h"
#include "../GEA2/Renderer/Chunk/Chunk.h"

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

class ChunkResourceManager
{
PUBLIC:

	ChunkResourceManager(Filesystem* filesystem);
	~ChunkResourceManager();

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
	std::mutex mutex;
	std::shared_ptr<File> file;
	MetaWorldHeader globalFileHeader;
};