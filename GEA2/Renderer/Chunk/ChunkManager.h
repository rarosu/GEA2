#pragma once
#include <string>
#include <vector>
#include "Chunk.h"
#include "../Camera.h"
#include <cstdint>
#include <ChunkResourceManager.h>
#include <map>

#define SCX 32
#define SCY 2
#define SCZ 32

class ChunkManager
{
public:
	ChunkManager(Filesystem* filesystem, MemoryAllocator* allocator, Camera* pcamera);
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
	static const int CHUNK_LOAD_DISTANCE = 16;

	Chunk* chunkList[SCX][SCY][SCZ];

	int nrOfChunks;
	int nrOfBlocks;

	Buffer worldMatBuf;

	ChunkResourceManager chunkResManager;

	std::vector<Resource<Chunk>> drawList;
	std::map<int, Resource<Chunk>> existMap;

	Camera* camera;
};