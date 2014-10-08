#pragma once
#include <string>
#include <vector>
#include "Chunk.h"
#include "../Camera.h"
#include <cstdint>
#include <ChunkResourceManager.h>
#include <ThreadPool.h>
#include <map>
#include <mutex>

class ChunkManager
{
public:
	ChunkManager(Filesystem* filesystem, MemoryAllocator* allocator, Camera* pcamera, const std::string& vWorldPath);
	~ChunkManager();
	uint8_t Get(int x, int y, int z);
	void Set(int x, int y, int z, uint8_t type);
	void Draw();
	void Update(float dt);

	void AddChunk(int x, int y, int z);
	void RemoveChunk(int x, int y, int z);

	int& GetNrOfBlocks();
	int GetNrOfChunks();

	//Quick and dirty block destroyer, remove if needed
	void DestroyBlock();
	
	

private:
	struct LoadChunkTask
	{
		Resource<Chunk> operator()(std::mutex* mutex, ChunkResourceManager* chunkLoader, int x, int y, int z)
		{
			Resource<Chunk> chunk = chunkLoader->Load(x, y, z);
			
			/*std::lock_guard<std::mutex> lock(*mutex);

			if (chunk->changed)
				chunk->UpdateChunk();*/
	
			return chunk;
		}
	};

	static const int CHUNK_LOAD_THREADS = 1;
	static const int CHUNK_LOAD_DISTANCE = 16;


	int nrOfChunks;
	int nrOfBlocks;

	Buffer worldMatBuf;

	ChunkResourceManager chunkResManager;
	ThreadPool chunkLoadPool;
	std::vector<std::pair<int, std::future<Resource<Chunk>>>> chunkFutures;
	std::vector<Resource<Chunk>> drawList;
	std::map<int, Resource<Chunk>> existMap;

	Camera* camera;
	std::mutex mutex;

	const MetaWorldHeader& metaHeader;
};