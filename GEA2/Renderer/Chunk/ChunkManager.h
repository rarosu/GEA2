#ifndef CMANAGER
#define CMANAGER

#include "d3dUtil.h"
#include "stdafx.h"
#include "Shader.h"
#include "Buffer.h"
#include <string>
#include <vector>
#include "Chunk.h"
#include <cstdint>
#include "simplexnoise.h"

#define SCX 16
#define SCY 8
#define SCZ 16

class ChunkManager
{
public:
	ChunkManager(ID3D10Device*	p_d3dDevice);
	~ChunkManager();
	uint8_t get(int x, int y, int z);
	void set(int x, int y, int z, uint8_t type);
	void render(D3DXMATRIX p_viewMatrix, D3DXMATRIX p_projMatrix);

	int getNrOfBlocks();
	int getNrOfChunks();

	void generateSimplexTerrain();

private:
	//DX10 device
	ID3D10Device*	m_d3dDevice;
	Chunk* m_chunkList[SCX][SCY][SCZ];
	Shader* m_shader;

	int m_nrOfChunks;
	int m_nrOfBlocks;
};

#endif