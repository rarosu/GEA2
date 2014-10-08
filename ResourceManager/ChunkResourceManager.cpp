#include "ChunkResourceManager.h"
#include <sstream>
#include "RLE/rle.c"

ChunkResourceManager::ChunkResourceManager(Filesystem* filesystem, MemoryAllocator* _allocator)
	: allocator(_allocator->GetInterface("ChunkResourceManager"))
{
	this->filesystem = filesystem;
	file = filesystem->GetFile("world.world");
	
	int header_size;
	file->Read(&header_size, sizeof(int));
	file->Read(header, header_size - sizeof(int));

	void* chunkMem = allocator.Alloc(sizeof(Chunk)* SCX * SCY * SCZ);

	pool = new(allocator.Alloc(sizeof(ThreadedPoolAllocator))) ThreadedPoolAllocator(chunkMem, sizeof(Chunk), SCX * SCY * SCZ);
}
ChunkResourceManager::~ChunkResourceManager()
{
	allocator.Free(chunkMem);
	allocator.Free(pool);
}

void ChunkResourceManager::Destructor(InternalResource<Chunk>* internal)
{
	pool->Free(internal->resource);

	chunks.RemoveResource(internal->hash);
}

Resource<Chunk> ChunkResourceManager::Load(int x, int y, int z)
{
	std::hash<std::string> hasher;
	std::stringstream ss;
	std::string filename("world.world");
	ss << filename << ',' << x << ',' << y << ',' << z;
	auto hash = hasher(ss.str());

	auto internal = chunks.GetResource(hash);
	if (internal != nullptr)
		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));

	{
		if (file == nullptr)
			return Resource<Chunk>();

		int i = SCZ * SCY * x + SCZ * y + z;
		uint8_t* compressed = new uint8_t[header[i].size];

		file->Seek(header[i].address, File::Origin::ORIGIN_BEG);
		file->Read(compressed, header[i].size);

		Chunk* chunk = new(pool->Alloc()) Chunk(glm::vec3(x * CX, y * CY, z * CZ));
		
		RLE_Uncompress(compressed, (unsigned char*)chunk->blockList, header[i].size);
		chunk->changed = true;

		delete[] compressed;

		internal = chunks.AddResource(hash, chunk);
		if (internal == nullptr)
			return Resource<Chunk>();

		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));
	}
}