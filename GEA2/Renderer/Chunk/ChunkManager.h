#pragma once
#include <string>
#include <vector>
#include "Chunk.h"
#include <cstdint>


#define SCX 32
#define SCY 2
#define SCZ 32

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

	void Export(const char* fileName);
	void Import(const char* fileName);
	void ImportChunkAt(int x, int y, int z);
private:
	Chunk* chunkList[SCX][SCY][SCZ];

	int nrOfChunks;
	int nrOfBlocks;

	Buffer worldMatBuf;

	//TESTING
	int xx, yy, zz, ii;
};