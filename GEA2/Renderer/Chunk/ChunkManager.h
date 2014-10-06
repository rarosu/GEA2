#pragma once
#include <string>
#include <vector>
#include "Chunk.h"
#include <cstdint>


#define SCX 1
#define SCY 1
#define SCZ 1

class ChunkManager
{
public:
	ChunkManager();
	~ChunkManager();
	uint8_t Get(int x, int y, int z);
	void Set(int x, int y, int z, uint8_t type);
	void Draw();

	int& GetNrOfBlocks();
	int GetNrOfChunks();

	void GenerateTerrain();

private:
	Chunk* chunkList[SCX][SCY][SCZ];

	int nrOfChunks;
	int nrOfBlocks;

	Buffer worldMatBuf;
};