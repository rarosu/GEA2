#pragma once
#include <string>
#include <vector>
#include "Chunk.h"
#include "../Camera.h"
#include <cstdint>
#include <ChunkResourceManager.h>
#include <map>

#define SCX 256
#define SCY 2
#define SCZ 256

class ChunkManager
{
public:
	ChunkManager(Filesystem* filesystem, Camera* pcamera);
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
	Chunk* chunkList[SCX][SCY][SCZ];

	int nrOfChunks;
	int nrOfBlocks;

	Buffer worldMatBuf;

	ChunkResourceManager chunkResManager;

	std::vector<Resource<Chunk>> drawList;
	std::map<int, Resource<Chunk>> existMap;

	Camera* camera;
};