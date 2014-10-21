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
	uint8_t Get(const glm::vec3& pos);
	uint8_t Get(int x, int y, int z);
	void Set(int x, int y, int z, uint8_t type);
	void Draw();
	void Update(float dt);

	void AddChunk(int x, int y, int z);
	void RemoveChunk(int x, int y, int z);

	int& GetNrOfBlocks();
	int& GetViewRadius();
	int& GetNrOfRenderedChunks();
	int& GetNrOfTasks();
	int GetNrOfChunks();

	//Get existing chunks
	std::map<int, Resource<Chunk>>* GetExistMap();

	//Quick and dirty block destroyer, remove if needed
	void DestroyBlock();

	
	

private:
	struct LoadChunkTask
	{
		Resource<Chunk> operator()(std::mutex* mutex, ChunkResourceManager* chunkLoader, int x, int y, int z, ChunkManager* loadedChunks)
		{
			Resource<Chunk> chunk = chunkLoader->Load(x, y, z);
			const MetaWorldHeader metaHeader = chunkLoader->GetGlobalWorldHeader();

			if (chunk != nullptr)
			{
				//Add neighbours to new chunk, not rebuilding neighbours, but still gives ~20% performance boost
				//Get pointer to exist map
				std::map<int, Resource<Chunk>>* existMap = loadedChunks->GetExistMap();

				//Right
				//Calculate offset position
				int pos = metaHeader.SCZ * metaHeader.SCY * (x+1) + metaHeader.SCZ * y + z;

				//Check if neighbour chunk exists
				auto it = existMap->find(pos);
				if (it != existMap->end())
				{
					//It exists, but is it loaded?
					if (it->second != nullptr)
					{
						chunk->right = it->second.Get();
					}
				}

				//Left
				//Calculate offset position
				pos = metaHeader.SCZ * metaHeader.SCY * (x - 1) + metaHeader.SCZ * y + z;

				//Check if neighbour chunk exists
				it = existMap->find(pos);
				if (it != existMap->end())
				{
					//It exists, but is it loaded?
					if (it->second != nullptr)
					{
						chunk->left = it->second.Get();
					}
				}

				//Front
				//Calculate offset position
				pos = metaHeader.SCZ * metaHeader.SCY * x + metaHeader.SCZ * y + (z-1);

				//Check if neighbour chunk exists
				it = existMap->find(pos);
				if (it != existMap->end())
				{
					//It exists, but is it loaded?
					if (it->second != nullptr)
					{
						chunk->front = it->second.Get();
					}
				}

				//Back
				//Calculate offset position
				pos = metaHeader.SCZ * metaHeader.SCY * x + metaHeader.SCZ * y + (z + 1);

				//Check if neighbour chunk exists
				it = existMap->find(pos);
				if (it != existMap->end())
				{
					//It exists, but is it loaded?
					if (it->second != nullptr)
					{
						chunk->back = it->second.Get();
					}
				}

				//Above
				//Calculate offset position
				pos = metaHeader.SCZ * metaHeader.SCY * x + metaHeader.SCZ * (y + 1) + z;

				//Check if neighbour chunk exists
				it = existMap->find(pos);
				if (it != existMap->end())
				{
					//It exists, but is it loaded?
					if (it->second != nullptr)
					{
						chunk->above = it->second.Get();
					}
				}

				//Below
				//Calculate offset position
				pos = metaHeader.SCZ * metaHeader.SCY * x + metaHeader.SCZ * (y - 1) + z;

				//Check if neighbour chunk exists
				it = existMap->find(pos);
				if (it != existMap->end())
				{
					//It exists, but is it loaded?
					if (it->second != nullptr)
					{
						chunk->below = it->second.Get();
					}
				}
				
				chunk->UpdateChunk(mutex);
			}
			
			return chunk;
		}
	};

	
	int chunkLoadDistance;
	int previousTotalChunkCount;

	int nrOfChunks;
	int nrOfBlocks;
	int nrOfRenderedChunks;
	int nrOfTasks;

	ChunkResourceManager chunkResManager;
	std::mutex mutex;
	ThreadPool chunkLoadPool;
	std::vector<std::pair<int, std::future<Resource<Chunk>>>> chunkFutures;
	std::vector<Resource<Chunk>> drawList;
	std::map<int, Resource<Chunk>> existMap;

	Camera* camera;
	

	const MetaWorldHeader& metaHeader;
};